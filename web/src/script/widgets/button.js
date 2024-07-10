import { Component } from "@alexgyver/component";
import { intToColor } from "./misc";
import WidgetEvent from "./event";

export default class Button {
    constructor(data) {
        this.id = data.id;
        Component.make('div', {
            context: this,
            var: 'root',
            class: 'button',
            style: {
                background: data.color ? intToColor(data.color) : 'var(--accent)',
            },
            child: {
                tag: 'div',
                children: [
                    {
                        tag: 'span',
                        text: data.label ?? 'Button'
                    },
                    {
                        tag: 'sup',
                        class: 'error_sup',
                        text: 'Error!',
                        var: 'error'
                    }
                ],
            },
            events: {
                click: () => this.$root.dispatchEvent(new WidgetEvent('click', this.id, null, this)),
            }
        });
    }

    setError() {
        this.$error.style.display = 'inline';
        setTimeout(() => {
            if (this.$error) this.$error.style.display = 'none';
        }, 2500);
    }
}