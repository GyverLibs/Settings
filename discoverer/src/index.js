import './index.css'
import { getIPs, getLocalIP, getMaskList } from './script/network';

document.addEventListener("DOMContentLoaded", async () => {
    let ip = await getLocalIP();
    console.log(ip);
    console.log(getMaskList());
    console.log(getIPs(ip, 24));

    const ips = getIPs(ip, 24);
    const delay = 100;
    for (let ip in ips) {
        discover(ips[ip], delay * ip).then(res => {
            if (res) console.log(res, ips[ip]);
        });
    }
});

function discover(ip, delay, timeout = 2000) {
    return new Promise((resolve) => {
        setTimeout(async () => {
            try {
                let res = await fetch(`http://${ip}/settings?action=discover`, { signal: AbortSignal.timeout(timeout) });
                if (res && res.ok) {
                    res = await res.json();
                    resolve(res);
                }
            } catch { }
            resolve(null);
        }, delay);
    });
}