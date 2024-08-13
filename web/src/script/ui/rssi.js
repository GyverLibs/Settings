import './rssi.css';

export function makeRSSI() {
    return `<svg viewBox="0 0 365.9 365.9" xml:space="preserve" xmlns="http://www.w3.org/2000/svg">
        <path name="3" d="M361 112a247 247 0 0 0-356 0 18 18 0 0 0 27 26 211 211 0 0 1 302 0 18 18 0 0 0 27-26z" style="fill:var(--font);fill-opacity:1"/>
        <path name="2" d="M183 107c-51 0-100 22-134 60a18 18 0 0 0 28 24 143 143 0 0 1 212 0 18 18 0 0 0 28-24c-34-38-83-60-134-60z" style="fill:var(--font);fill-opacity:1"/>
        <path name="1" d="M183 176c-36 0-69 17-90 46a18 18 0 0 0 30 22 74 74 0 0 1 120 0 18 18 0 0 0 30-22c-21-29-54-46-90-46z" style="fill:var(--font);fill-opacity:1"/>
        <circle name="0" cx="182.9" cy="286.7" r="41.5" style="fill:var(--font);fill-opacity:1"/>
        <path name="x" d="M273 127a19 19 0 0 0-27-27l-63 63-63-63a19 19 0 0 0-28 27l64 64-64 63a19 19 0 0 0 28 27l63-63 63 63a19 19 0 0 0 27-27l-63-63z" style="fill:var(--error);fill-opacity:0"/>
    </svg>`;
}

export function changeRSSI(cont, rssi) {
    cont.title = rssi + '%';
    let svg = cont.firstElementChild;
    const min = 0.2, max = 0.7;
    for (let p of svg.children) {
        switch (p.getAttribute('name')) {
            case '3': p.style.fillOpacity = rssi > 75 ? max : min; break;
            case '2': p.style.fillOpacity = rssi > 50 ? max : min; break;
            case '1': p.style.fillOpacity = rssi > 25 ? max : min; break;
            case '0': p.style.fillOpacity = rssi > 0 ? max : min; break;
            case 'x': p.style.fillOpacity = rssi == 0 ? 1 : 0; break;
        }
    }
}