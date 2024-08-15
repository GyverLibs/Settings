import './rssi.css';

export function makeRSSI() {
    return `<svg xml:space="preserve" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 366.5 256">
        <path name="3" d="M361 91A247 247 0 0 0 5 91a19 19 0 0 0 27 26 211 211 0 0 1 302 0 19 19 0 0 0 27-26z" style="fill:var(--font)"/>
        <path name="2" d="M183 86c-51 0-100 22-134 60a18 18 0 0 0 28 24 143 143 0 0 1 212 0 18 18 0 0 0 28-24c-34-38-83-60-134-60z" style="fill:var(--font)"/>
        <path name="1" d="M183 155c-36 0-69 17-90 46a19 19 0 0 0 30 22 74 74 0 0 1 120 0 19 19 0 0 0 30-22c-21-29-54-46-90-46z" style="fill:var(--font)"/>
        <path name="x" d="M292 147a19 19 0 0 0 0-38h-89V20a19 19 0 0 0-39-1v91l-90-1a19 19 0 0 0 1 39h89v89a19 19 0 0 0 38 0v-89z" style="fill:var(--error);fill-opacity:0"/>
    </svg>`;
}

export function changeRSSI(cont, rssi) {
    let apply = (path, state) => {
        path.style.fillOpacity = state ? 0.8 : 0.2;
    }
    let error = (path, rssi) => {
        path.style.fillOpacity = rssi ? 0 : 1.0;
    }

    cont.title = rssi + '%';
    let svg = cont.firstElementChild;
    for (let p of svg.children) {
        switch (p.getAttribute('name')) {
            case '3': apply(p, rssi > 66); break;
            case '2': apply(p, rssi > 33); break;
            case '1': apply(p, rssi > 0); break;
            case 'x': error(p, rssi); break;
        }
    }
}