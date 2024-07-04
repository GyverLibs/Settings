const codes = [
    'id',
    'type',
    'content',
    'group',
    'menu',
    'buttons',

    'build',
    'reload',
    'update',
    'ping',

    'label',
    'title',
    'text',
    'value',
    'color',
    'min',
    'max',
    'step',
    'unit',

    'input',
    'pass',
    'select',
    'slider',
    'toggle',
    'time',
    'date',
    'datetime',
    'button',
];

export default function decodeBson(b) {
    if (!b || !(b instanceof Uint8Array) || !b.length) return null;

    const BS_KEY_CODE = '$';
    const BS_KEY_STR8 = 'k';
    const BS_KEY_STR16 = 'K';

    const BS_DATA_CODE = '#';
    const BS_DATA_FLOAT = 'f';
    const BS_DATA_STR8 = 's';
    const BS_DATA_STR16 = 'S';

    const BS_DATA_INT8 = '1';
    const BS_DATA_INT16 = '2';
    const BS_DATA_INT32 = '4';
    const BS_DATA_INT64 = '8';

    const BS_DATA_UINT8 = 'b';
    const BS_DATA_UINT16 = 'c';
    const BS_DATA_UINT32 = 'e';
    const BS_DATA_UINT64 = 'i';

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

    let s = '';
    for (let i = 0; i < b.length; i++) {
        let c = String.fromCharCode(b[i]);
        switch (c) {
            case ']':
            case '}':
                if (s[s.length - 1] == ',') s = s.slice(0, -1);
                s += c;
                s += ',';
                break;

            case '[':
            case '{':
                s += c;
                break;

            case BS_DATA_CODE:
            case BS_KEY_CODE: {
                s += '"' + codes[b[++i]] + '"';
                s += (c == BS_KEY_CODE) ? ':' : ',';
            } break;

            case BS_KEY_STR8:
            case BS_DATA_STR8:
            case BS_KEY_STR16:
            case BS_DATA_STR16: {
                let key = (c == BS_KEY_STR8 || c == BS_KEY_STR16);
                let len = b[++i];
                if (c == BS_KEY_STR16 || c == BS_DATA_STR16) {
                    len <<= 8;
                    len |= b[++i];
                }
                i++;
                len = (len >>> 0);
                s += '"';
                s += new TextDecoder().decode(b.slice(i, i + len));
                i += len - 1;
                s += '"';
                s += key ? ':' : ',';
            } break;

            case BS_DATA_INT8:
            case BS_DATA_INT16:
            case BS_DATA_INT32:
            case BS_DATA_UINT8:
            case BS_DATA_UINT16:
            case BS_DATA_UINT32: {
                let size = 0;
                switch (c) {
                    case BS_DATA_INT8:
                    case BS_DATA_INT16:
                    case BS_DATA_INT32:
                        size = b[i] - '0'.charCodeAt(0);
                        break;
                    default:
                        size = b[i] - 'a'.charCodeAt(0);
                        break;
                }
                let v = 0;
                while (size--) {
                    v <<= 8;
                    v |= b[++i];
                }
                switch (c) {
                    case BS_DATA_INT8: s += (new Int8Array([v]))[0]; break;
                    case BS_DATA_INT16: s += (new Int16Array([v]))[0]; break;
                    case BS_DATA_INT32: s += (new Int32Array([v]))[0]; break;
                    case BS_DATA_UINT8: s += (new Uint8Array([v]))[0]; break;
                    case BS_DATA_UINT16: s += (new Uint16Array([v]))[0]; break;
                    case BS_DATA_UINT32: s += (new Uint32Array([v]))[0]; break;
                }
                s += ',';
            } break;

            case BS_DATA_INT64:
            case BS_DATA_UINT64:
                let size = 8;
                let v = BigInt(0);
                while (size--) {
                    v <<= 8n;
                    v |= BigInt(b[++i]);
                }
                s += '"' + v + '"';
                s += ',';
                break;

            case BS_DATA_FLOAT: {
                let v = 0;
                let dec = b[++i];
                let size = 4;
                while (size--) {
                    v <<= 8;
                    v |= b[++i];
                }
                s += ieee32ToFloat(v).toFixed(dec);
                s += ',';
            } break;
        }
    }
    if (s[s.length - 1] == ',') s = s.slice(0, -1);
    
    try {
        return JSON.parse(s);
    } catch (e) {
        throw new Error("JSON error")
    }
}