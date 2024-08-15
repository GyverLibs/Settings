export default function decodeBson(b, codes = []) {
    if (!b || !(b instanceof Uint8Array)) return null;
    if (!b.length) return {};
    let bins = [];

    const BS_KEY_CODE = (0 << 5);
    const BS_KEY_STR = (1 << 5);
    const BS_VAL_CODE = (2 << 5);
    const BS_VAL_STR = (3 << 5);
    const BS_VAL_INT = (4 << 5);
    const BS_VAL_FLOAT = (5 << 5);
    const BS_CONTAINER = (6 << 5);
    const BS_BINARY = (7 << 5);
    const BS_BIN_PREFIX = "__BSON_BINARY";

    const BS_CONT_OBJ = (1 << 4);
    const BS_CONT_OPEN = (1 << 3);

    function ieee32ToFloat(intval) {
        var fval = 0.0;
        var x;
        var m;
        var s;
        s = (intval & 0x80000000) ? -1 : 1;
        x = ((intval >> 23) & 0xFF);
        m = (intval & 0x7FFFFF);
        switch (x) {
            case 0:
                break;
            case 0xFF:
                if (m) fval = NaN;
                else if (s > 0) fval = Number.POSITIVE_INFINITY;
                else fval = Number.NEGATIVE_INFINITY;
                break;
            default:
                x -= 127;
                m += 0x800000;
                fval = s * (m / 8388608.0) * Math.pow(2, x);
                break;
        }
        return fval;
    }
    function unpack5(msb5, lsb) {
        return ((msb5 << 8) | lsb) >>> 0;
    }
    function makeBins(obj) {
        if (typeof obj !== 'object') return;
        for (let k in obj) {
            if (typeof obj[k] === "object" && obj[k] !== null) {
                makeBins(obj[k]);
            } else if (typeof obj[k] === "string" && obj[k].startsWith(BS_BIN_PREFIX)) {
                obj[k] = bins[obj[k].split('#')[1]];
            }
        }
    }

    let s = '';
    for (let i = 0; i < b.length; i++) {
        const type = b[i] & 0b11100000;
        const data = b[i] & 0b00011111;

        switch (type) {
            case BS_CONTAINER:
                if (data & BS_CONT_OPEN) {
                    s += (data & BS_CONT_OBJ) ? '{' : '[';
                } else {
                    if (s[s.length - 1] == ',') s = s.slice(0, -1);
                    s += (data & BS_CONT_OBJ) ? '}' : ']';
                    s += ',';
                }
                break;

            case BS_KEY_CODE:
            case BS_VAL_CODE:
                s += '"' + codes[unpack5(data, b[++i])] + '"';
                s += (type == BS_KEY_CODE) ? ':' : ',';
                break;

            case BS_KEY_STR:
            case BS_VAL_STR: {
                let len = unpack5(data, b[++i]);
                i++;
                let txt = new TextDecoder().decode(b.slice(i, i + len));
                txt = txt.replaceAll(/([^\\])\\([^\"\\nrt])/ig, "$1\\\\$2")
                    .replaceAll(/\t/ig, "\\t")
                    .replaceAll(/\n/ig, "\\n")
                    .replaceAll(/\r/ig, "\\r")
                    .replaceAll(/([^\\])(")/ig, '$1\\"');
                s += '"' + txt + '"';
                s += (type == BS_KEY_STR) ? ':' : ',';
                i += len - 1;
            } break;

            case BS_VAL_INT: {
                if (data & 0b10000) s += '-';
                let len = data & 0b01111;
                let v = BigInt(0);
                for (let j = 0; j < len; j++) {
                    v |= BigInt(b[++i]) << BigInt(j * 8);
                }
                s += v;
                s += ',';
            } break;

            case BS_VAL_FLOAT: {
                let v = 0;
                for (let j = 0; j < 4; j++) {
                    v |= b[++i] << (j * 8);
                }
                let f = ieee32ToFloat(v);
                if (isNaN(f)) {
                    s += '"NaN"';
                } else if (!isFinite(f)) {
                    s += '"Infinity"';
                } else {
                    s += f.toFixed(data);
                }
                s += ',';
            } break;

            case BS_BINARY: {
                let len = unpack5(data, b[++i]);
                i++;
                s += '"' + BS_BIN_PREFIX + '#' + bins.length + '",';
                bins.push(b.slice(i, i + len));
                i += len - 1;
            } break;
        }
    }
    if (s[s.length - 1] == ',') s = s.slice(0, -1);

    try {
        let obj = JSON.parse(s);
        if (bins.length) makeBins(obj);
        return obj;
    } catch (e) {
        throw new Error("JSON error")
    }
}