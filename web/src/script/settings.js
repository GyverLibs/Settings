import { Component } from '@alexgyver/component';
import Page from './page';
import { Arrow } from './widgets/misc';
import popup from './popup';
import decodeBson from './bson';
import unMap from './unmap';

const timeout = 2000;

export default class Settings {
    pageStack = [];
    widgets = new unMap();
    cfg = { dark: false };
    pingprd = null;
    offline = false;

    constructor() {
        this.$arrow = Arrow('left', 16);

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
                                    tag: 'sup',
                                    class: 'error_sup',
                                    text: 'Offline!',
                                    var: 'offline',
                                }
                            ]
                        },
                        {
                            tag: 'div',
                            class: 'moon',
                            events: {
                                click: () => {
                                    document.body.classList.toggle('theme_dark');
                                    this.cfg.dark = document.body.classList.contains('theme_dark');
                                    this.updateCfg();
                                },
                            }
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
            e.detail.page.style.display = 'block';
            e.detail.parent.style.display = 'none';
            this.pageStack.push({ page: e.detail.page, title: e.detail.label });
            this.$title.innerText = e.detail.label;
            this.$title.style.cursor = 'pointer';
            this.$arrow.style.display = 'block';
        });

        this.$main_col.addEventListener("widget_event", async (e) => {
            let res;
            switch (e.data.action) {
                case 'click': res = await this.send('click', e.data.id); break;
                case 'set': res = await this.send('set', e.data.id, e.data.value); break;
            }
            if (res == null) e.data.widget.setError();
            else this.parse(res);
        });

        this.$arrow.addEventListener('click', () => this.back());
        this.$title.addEventListener('click', () => this.back());

        window.history.pushState({ page: 1 }, "", "");
        window.onpopstate = () => {
            window.history.pushState({ page: 1 }, "", "");
            this.back();
        }

        if (localStorage.hasOwnProperty('cfg')) {
            let newcfg = JSON.parse(localStorage.getItem('cfg'));
            this.cfg = Object.assign({}, this.cfg, newcfg);
        }
        if (this.cfg.dark) document.body.classList.add('theme_dark');

        if (localStorage.hasOwnProperty('cache')) {
            this.render(JSON.parse(localStorage.getItem('cache')));
        }
        this.reload();
    }

    async reload() {
        this.parse(await this.send('load'));
    }

    async send(action, id = null, value = null) {
        let url = window.location.origin + '/settings?action=' + action;
        // let url = 'http://192.168.1.130/settings?action=' + action;
        if (id) url += '&id=' + id;
        if (value != null) url += '&value=' + value;

        let res = null;
        try {
            res = await fetch(url, { signal: AbortSignal.timeout(timeout) });
        } catch (e) { }

        if (!res || !res.ok) return null;

        try {
            return decodeBson(new Uint8Array(await res.arrayBuffer()));
        } catch (e) {
            popup(e);
        }
        return null;
    }

    back() {
        if (this.pageStack.length > 1) {

            this.pageStack.pop().page.style.display = 'none';
            this.pageStack[this.pageStack.length - 1].page.style.display = 'block';

            this.$title.innerText = this.pageStack[this.pageStack.length - 1].title;
            this.$arrow.style.display = 'block';
        }
        if (this.pageStack.length == 1) {
            this.$arrow.style.display = 'none';
            this.$title.style.cursor = 'default';
        }
    }

    restartPing(prd) {
        if (this.pingprd) clearInterval(this.pingprd);
        this.pingprd = setInterval(async () => {
            const res = await this.send(this.offline ? 'load' : 'ping');
            if (res) {
                this.offline = false;
                this.parse(res);
            } else {
                this.offline = true;
            }
            this.$offline.style.display = this.offline ? 'inline' : 'none';
        }, prd);
    }

    parse(packet) {
        if (!packet) return;

        switch (packet.type) {
            case 'build':
                this.render(packet);
                localStorage.setItem('cache', JSON.stringify(packet));
                this.restartPing(packet.ping);
                break;

            case 'update':
                for (let upd of packet.content) {
                    if (this.widgets.has(upd.id)) this.widgets.get(upd.id).update(upd.value);
                }
                break;
        }
    }

    render(json) {
        this.$title.innerText = json.title ?? 'Settings';
        document.title = this.$title.innerText;
        let pages = [];
        this.widgets = new unMap();
        Page(json.content, pages, this.widgets);
        pages[0].style.display = 'block';
        this.$main_col.replaceChildren(...pages);
        this.pageStack = [{ page: pages[0], title: json.title }];
    }

    updateCfg() {
        localStorage.setItem('cfg', JSON.stringify(this.cfg));
    }
};