export function getLocalIP() {
    return new Promise(res => {
        const RTCPeerConnection = window.RTCPeerConnection || window.webkitRTCPeerConnection || window.mozRTCPeerConnection;
        if (!RTCPeerConnection) res(null);

        const rtc = new RTCPeerConnection({ iceServers: [] });
        rtc.createDataChannel('', { reliable: false });

        rtc.addEventListener("icecandidate", evt => {
            if (evt.candidate && evt.candidate.address) {
                if (evt.candidate.address.endsWith('.local')) {
                    alert(`Disable WEB RTC anonymizer: ${browser()}:/` + `/flags/#enable-webrtc-hide-local-ips-with-mdns`);
                } else {
                    res(evt.candidate.address);
                }
            }
            res(null);
        });

        rtc.createOffer().then(offerDesc => rtc.setLocalDescription(offerDesc));
    });
}

export function getIPs(ip, netmask = 24) {
    let ip_a = ip.split('.');
    let sum_ip = (ip_a[0] << 24) | (ip_a[1] << 16) | (ip_a[2] << 8) | ip_a[3];
    let cidr = Number(netmask);
    let mask = ~(0xffffffff >>> cidr);
    let network = 0, broadcast = 0, start_ip = 0, end_ip = 0;
    if (cidr === 32) {
        network = sum_ip;
        broadcast = network;
        start_ip = network;
        end_ip = network;
    } else {
        network = sum_ip & mask;
        broadcast = network + (~mask);
        if (cidr === 31) {
            start_ip = network;
            end_ip = broadcast;
        } else {
            start_ip = network + 1;
            end_ip = broadcast - 1;
        }
    }
    let ips = ['192.168.4.1'];  // esp
    for (let ip = start_ip; ip <= end_ip; ip++) {
        ips.push(`${(ip >>> 24) & 0xff}.${(ip >>> 16) & 0xff}.${(ip >>> 8) & 0xff}.${ip & 0xff}`);
    }
    return ips;
}

export function getMaskList() {
    const list = [];
    for (let i = 0; i < 33; i++) {
        let imask;
        if (i == 32) imask = 0xffffffff;
        else imask = ~(0xffffffff >>> i);
        list.push(`${(imask >>> 24) & 0xff}.${(imask >>> 16) & 0xff}.${(imask >>> 8) & 0xff}.${imask & 0xff}`);
    }
    return list;
}

export function browser() {
    if (navigator.userAgent.includes("Opera") || navigator.userAgent.includes('OPR')) return 'opera';
    else if (navigator.userAgent.includes("Edg")) return 'edge';
    else if (navigator.userAgent.includes("Chrome")) return 'chrome';
    else if (navigator.userAgent.includes("Safari")) return 'safari';
    else if (navigator.userAgent.includes("Firefox")) return 'firefox';
    else if ((navigator.userAgent.includes("MSIE")) || (!!document.documentMode == true)) return 'IE';
    else return 'unknown';
}