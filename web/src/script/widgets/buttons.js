import { Component } from "@alexgyver/component";
import { intToColor } from "./misc";

export default class ButtonsWidget {
    id = 0;
    
    constructor(data) {
        let buttons = [];
        for (let b of data.content) {
            let btn = Component.make('div', {
                class: 'button',
                style: {
                    background: b.color ? intToColor(b.color) : 'var(--accent)',
                },
                child: {
                    tag: 'span',
                    text: b.label ?? 'Button',
                }
            });
            btn.addEventListener('click', () => {
                btn.dispatchEvent(new CustomEvent("bclick", {
                    bubbles: true,
                    detail: { id: b.id }
                }));
            });
            buttons.push(btn);
        }

        this.$root = Component.make('div', {
            class: 'buttons',
            children: buttons,
        });
    }
};