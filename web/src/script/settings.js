import { Component } from '@alexgyver/component';
import Page from './page';
import { Arrow, hash, http_post } from './utils';
import popup from './ui/popup';
import decodeBson from './bson';
import { codes } from './codes';
import unMap from './unmap';
import { AsyncConfirm, AsyncPrompt } from './ui/dialog';
import { changeRSSI, makeRSSI } from './ui/rssi';

const timeout = 2000;
const anim_s = '.11s';
const anim_ms = 100;

function iconGradient(icon, perc) {
    icon.style.background = 'none';
    icon.style.backgroundImage = `linear-gradient(90deg,var(--accent) ${perc}%, var(--font_tint) 0%)`;
}
function iconFill(icon, color) {
    icon.style.background = 'none';
    icon.style.backgroundColor = color;
}

export default class Settings {
    pageStack = [];
    widgets = new unMap();
    ping_int = null;
    offline = false;
    transOn = null;
    transOut = null;
    authF = false;
    granted = false;
    firstBuild = true;
    auth = 0;
    ping_prd = 2000;

    constructor() {
        this.$arrow = Arrow('left', 16, {
            display: 'none',
            marginRight: '4px',
        });

        Component.make('div', {
            parent: document.body,
            context: this,
            class: 'main',
            children: [
                {
                    tag: 'div',
                    class: 'header',
                    children: [
                        {
                            tag: 'div',
                            children: [
                                {
                                    tag: 'div',
                                    class: 'nav',
                                    children: [
                                        this.$arrow,
                                        {
                                            tag: 'span',
                                            var: 'title'
                                        }
                                    ]
                                },
                                {
                                    tag: 'div',
                                    class: 'rssi',
                                    var: 'rssi',
                                    html: makeRSSI(),
                                }
                            ]
                        },
                        {
                            tag: 'div',
                            class: 'icon bars menubutton',
                            var: 'menubutton',
                            events: {
                                click: async () => {
                                    iconFill(this.$ota, 'var(--font_tint)');
                                    iconFill(this.$upload, 'var(--font_tint)');
                                    this.$main_col.classList.toggle('hidden');
                                    this.$main_menu.classList.toggle('hidden');
                                    if (this.$main_menu.classList.contains('hidden')) {
                                        this.parse(await this.send('load'));
                                        this.$menubutton.classList = 'icon bars menubutton';
                                    } else {
                                        this.parse(await this.send('fs'));
                                        this.$menubutton.classList = 'icon cross menubutton';
                                    }
                                },
                            }
                        },
                    ]
                },
                {
                    tag: 'div',
                    class: 'group_col hidden',
                    var: 'main_menu',
                    style: { marginTop: '10px' },
                    children: [
                        {
                            tag: 'div',
                            class: 'menu_icons',
                            children: [
                                {
                                    tag: 'div',
                                    class: 'menu_icon',
                                    child: {
                                        tag: 'div',
                                        class: 'icon moon',
                                        title: 'Dark mode',
                                        events: {
                                            click: () => {
                                                document.body.classList.toggle('theme_dark');
                                                localStorage.setItem('dark', document.body.classList.contains('theme_dark') ? 1 : 0);
                                            },
                                        }
                                    }
                                },
                                {

                                    tag: 'div',
                                    class: 'menu_icon',
                                    child: {
                                        tag: 'div',
                                        class: 'icon key',
                                        title: 'Auth',
                                        var: 'auth',
                                    }
                                },
                                {
                                    tag: 'input',
                                    type: 'file',
                                    var: 'upload_ota',
                                    style: 'display: none',
                                    accept: '.bin',
                                    events: {
                                        change: () => this.uploadOta(this.$upload_ota.files[0]),
                                    }
                                },
                                {
                                    tag: 'div',
                                    class: 'menu_icon drop_area',
                                    events: {
                                        drop: (e) => this.uploadOta(e.dataTransfer.files[0]),
                                    },
                                    child: {
                                        tag: 'div',
                                        class: 'icon cloud',
                                        title: 'OTA',
                                        var: 'ota',
                                        events: {
                                            click: () => this.$upload_ota.click(),
                                        }
                                    }
                                },
                                {
                                    tag: 'input',
                                    type: 'file',
                                    var: 'upload_file',
                                    style: 'display: none',
                                    events: {
                                        change: () => this.uploadFile(this.$upload_file.files[0]),
                                    }
                                },
                                {
                                    tag: 'div',
                                    class: 'menu_icon drop_area',
                                    events: {
                                        drop: (e) => this.uploadFile(e.dataTransfer.files[0]),
                                    },
                                    child: {
                                        tag: 'div',
                                        class: 'icon upload',
                                        title: 'Upload',
                                        var: 'upload',
                                        events: {
                                            click: () => this.$upload_file.click(),
                                        }
                                    }
                                },
                            ]
                        },
                        {
                            tag: 'div',
                            class: 'fs_cont',
                            var: 'fs',
                        }
                    ]
                },
                {
                    tag: 'div',
                    class: 'main_col',
                    var: 'main_col'
                }
            ]
        });

        this.$main_col.addEventListener("menuclick", (e) => {
            window.scrollTo(0, 0);
            e.detail.page.style.display = 'block';
            e.detail.page.style.animation = 'shiftLeft ' + anim_s;
            e.detail.parent.style.animation = 'fadeOut ' + anim_s;
            this.$main_col.style.minHeight = Math.max(e.detail.parent.offsetHeight, e.detail.page.offsetHeight) + 'px';

            setTimeout(() => {
                e.detail.parent.style.display = 'none';
                this.$main_col.style.minHeight = e.detail.page.offsetHeight + 'px';
            }, anim_ms);

            this.pageStack.push({ page: e.detail.page, title: e.detail.label });
            this.$title.innerText = e.detail.label;
            this.$title.style.cursor = 'pointer';
            this.$arrow.style.display = 'block';
        });

        this.$main_col.addEventListener("widget_event", async (e) => {
            let res = null;
            switch (e.data.action) {
                case 'click': res = await this.send('click', e.data.id); break;
                case 'set': res = await this.send('set', e.data.id, e.data.value); break;
            }
            this.parse(res);
            if (res === null) e.data.widget.setError();
        });

        this.$arrow.addEventListener('click', () => this.back());
        this.$title.addEventListener('click', () => this.back());

        window.history.pushState({ page: 1 }, "", "");
        window.onpopstate = () => {
            window.history.pushState({ page: 1 }, "", "");
            this.back();
        }

        ['dragenter', 'dragover', 'dragleave', 'drop'].forEach(ev => {
            document.body.addEventListener(ev, (e) => {
                e.preventDefault();
                e.stopPropagation();
            }, false);
        });
        ['dragenter', 'dragover'].forEach(e => {
            document.body.addEventListener(e, function () {
                document.querySelectorAll('.drop_area').forEach((el) => el.classList.add('active'));
            }, false);
        });
        ['dragleave', 'drop'].forEach(e => {
            document.body.addEventListener(e, function () {
                document.querySelectorAll('.drop_area').forEach((el) => el.classList.remove('active'));
            }, false);
        });

        if (localStorage.hasOwnProperty('dark')) {
            if (Number(localStorage.getItem('dark'))) {
                document.body.classList.add('theme_dark');
            }
        }

        if (localStorage.hasOwnProperty('auth')) {
            this.auth = Number(localStorage.getItem('auth'));
        }

        if (localStorage.hasOwnProperty('cache')) {
            this.renderUI(JSON.parse(localStorage.getItem('cache')));
        }
        this.load();
    }

    async load() {
        const res = await this.send('load');
        this.parse(res);
        if (!res) {
            this.setOffline(true);
            this.restartPing();
        }
    }

    async send(action, id = null, value = null) {
        let res = null;
        try {
            res = await fetch(this.makeUrl('settings', { action: action, id: id, value: value }), { signal: AbortSignal.timeout(timeout) });
        } catch (e) { }

        if (!res || !res.ok) return null;

        try {
            return decodeBson(new Uint8Array(await res.arrayBuffer()), codes);
        } catch (e) {
            popup(e);
        }
        return null;
    }

    back() {
        let dialogs = document.querySelectorAll('.dialog_back');
        if (dialogs.length) {
            dialogs[dialogs.length - 1].remove();
            return;
        }

        if (this.pageStack.length > 1) {
            window.scrollTo(0, 0);
            let right = this.pageStack.pop().page;
            right.style.animation = 'shiftRight ' + anim_s;

            let fadein = this.pageStack[this.pageStack.length - 1].page;
            fadein.style.display = 'block';
            fadein.style.animation = 'fadeIn ' + anim_s;
            this.$main_col.style.minHeight = Math.max(fadein.offsetHeight, right.offsetHeight) + 'px';

            setTimeout(() => {
                right.style.display = 'none';
                this.$main_col.style.minHeight = fadein.offsetHeight + 'px';
            }, anim_ms);

            this.$title.innerText = this.pageStack[this.pageStack.length - 1].title;
            this.$arrow.style.display = 'block';
        }
        if (this.pageStack.length == 1) {
            this.$arrow.style.display = 'none';
            this.$title.style.cursor = 'default';
        }
    }

    restartPing() {
        this.stopPing();
        if (!this.ping_prd) return;
        this.ping_int = setInterval(async () => {
            const res = await this.send(this.offline ? 'load' : 'ping');
            this.parse(res);
            if (!res) this.authF = false;
            this.setOffline(!res);
        }, this.ping_prd);
    }
    stopPing() {
        if (this.ping_int) clearInterval(this.ping_int);
        this.ping_int = null;
    }
    setOffline(offline) {
        this.offline = offline;
        if (offline) changeRSSI(this.$rssi, 0);
    }

    parse(packet) {
        if (!packet) return;

        switch (packet.type) {
            case 'build':
                this.renderUI(packet);
                localStorage.setItem('cache', JSON.stringify(packet));
                this.ping_prd = packet.ping;
                this.restartPing();

                if (!this.authF) {
                    this.authF = true;
                    if ('granted' in packet) {
                        this.granted = packet.granted;
                        this.$auth.style.backgroundColor = this.granted ? 'var(--accent)' : 'var(--error)';
                        this.$ota.style.display = this.granted ? 'inline-block' : 'none';
                        this.$upload.style.display = this.granted ? 'inline-block' : 'none';
                        if (!this.granted && this.firstBuild) popup('Unauthorized');
                        this.$auth.addEventListener('click', async () => {
                            let res = await AsyncPrompt('Password', '');
                            if (res !== null) {
                                localStorage.setItem('auth', hash(res));
                                window.location.reload();
                            }
                        });
                    } else {
                        this.$auth.style.backgroundColor = 'var(--font_tint)';
                        this.granted = true;
                    }
                }
                if (packet.gzip) this.$upload_ota.accept = '.gz';
                this.firstBuild = false;
                break;

            case 'update':
                for (let upd of packet.content) {
                    if (this.widgets.has(upd.id)) this.widgets.get(upd.id).update(upd.value);
                }
                break;

            case 'fs':
                this.renderFS(packet);
                break;
        }
        if (packet.rssi) changeRSSI(this.$rssi, packet.rssi);
    }

    renderUI(json) {
        this.$title.innerText = json.title ?? 'Settings';
        document.title = this.$title.innerText;
        let pages = [];
        this.widgets = new unMap();
        Page(json.content, pages, this.widgets);
        this.$main_col.replaceChildren(...pages);
        this.pageStack = [{ page: pages[0], title: json.title }];
        pages[0].style.display = 'block';
        this.$main_col.style.minHeight = pages[0].offsetHeight + 'px';
    }
    renderFS(packet) {
        this.$fs.replaceChildren();
        if (packet.error) {
            this.$fs.append(Component.make('div', { class: 'fs_error', text: packet.error }));
        } else {
            let fs = packet.content.split(';');
            for (let ps of fs) {
                ps = ps.split(':');
                let path = ps[0];
                let size = ps[1];
                if (path.length) Component.make('div', {
                    class: 'fs_row',
                    parent: this.$fs,
                    children: [
                        {
                            tag: 'span',
                            class: 'fs_path',
                            text: path,
                            events: {
                                click: () => this.fetchFile(path),
                            }
                        },
                        {
                            tag: 'div',
                            class: 'fs_row',
                            style: { padding: '0' },
                            children: [
                                {
                                    tag: 'span',
                                    class: 'fs_size',
                                    text: (size / 1024).toFixed(1) + 'k',
                                },
                                {
                                    tag: 'div',
                                    class: 'icon cross',
                                    style: { width: '17px', height: '17px', background: 'var(--error)' },
                                    events: {
                                        click: async () => {
                                            if (await AsyncConfirm("Remove " + path + "?")) {
                                                this.removeFile(path);
                                            }
                                        }
                                    }
                                }
                            ]
                        }
                    ],
                });
            }
            Component.make('div', {
                parent: this.$fs,
                class: 'fs_info',
                style: {
                    backgroundImage: `linear-gradient(90deg,var(--accent) ${packet.used / packet.total * 100}%, var(--shadow_light) 0%)`,
                },
                text: 'Used: ' + `${(packet.used / 1000).toFixed(2)}/${(packet.total / 1000).toFixed(2)} kB [${Math.round(packet.used / packet.total * 100)}%]`,
            });
        }
    }

    async removeFile(path) {
        let res = await this.send('remove', 0, path);
        this.parse(res);
    }
    async fetchFile(path) {
        try {
            let res = await fetch(this.makeUrl('fetch', { path: path }), { signal: AbortSignal.timeout(timeout) });
            res = await res.blob();

            let link = document.createElement('a');
            link.href = window.URL.createObjectURL(res);
            link.download = path.split('/').slice(-1);
            link.click();
        } catch (e) {
            popup(e);
        }
    }
    async uploadFile(file) {
        this.$upload_file.value = "";
        let path = await AsyncPrompt("Upload", '/' + file.name);
        if (!path) return;

        this.stopPing();
        let data = new FormData();
        data.append('upload', file);
        let ok = false;
        try {
            let res = await http_post(this.makeUrl('upload', { path: path }), data, (perc) => iconGradient(this.$upload, perc));
            if (res == 200) ok = true;
        } catch (e) { }
        iconFill(this.$upload, ok ? 'var(--font_tint)' : 'var(--error)');
        popup(ok ? 'Upload done' : 'Upload error', !ok);
        if (ok) this.parse(await this.send('fs'));
        this.restartPing();
    }
    async uploadOta(file) {
        if (!file.name.endsWith(this.$upload_ota.accept)) return;
        if (!await AsyncConfirm("Update firmware?")) return;
        this.stopPing();
        this.offline = true;

        this.$upload_ota.value = "";

        let data = new FormData();
        data.append('ota', file);
        let ok = false;

        try {
            let res = await http_post(this.makeUrl('ota'), data, (perc) => iconGradient(this.$ota, perc));
            if (res == 200) ok = true;
        } catch (e) { }

        iconFill(this.$ota, ok ? 'var(--font_tint)' : 'var(--error)');
        popup(ok ? 'OTA done' : 'OTA error', !ok);
        this.restartPing();
    }
    makeUrl(cmd, params = {}) {
        // const base_url = 'http://192.168.1.54';
        const base_url = window.location.origin;

        if (this.auth) params.auth = this.auth.toString(16);
        let url = base_url + '/' + cmd;
        let first = true;
        for (let p in params) {
            if (params[p] === null) continue;
            url += first ? '?' : '&';
            first = false;
            url += (p + '=' + params[p]);
        }
        return url;
    }
};