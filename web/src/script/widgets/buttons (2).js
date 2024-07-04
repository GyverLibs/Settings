import { Component } from "@alexgyver/component";
import { intToColor } from "./misc";
import WidgetEvent from "./event";

class Button {
    constructor(data) {
        this.id = data.id;
        Component.make('div', {
            context: this,
            var: 'btn',
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
                click: () => this.$btn.dispatchEvent(new WidgetEvent('click', this.id, null, this)),
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

export default class ButtonsWidget {
    $root;
    buttons = [];

    constructor(data) {
        for (let b of data.content) {
            this.buttons.push(new Button(b));
        }

        this.$root = Component.make('div', {
            class: 'buttons',
            children: this.buttons.map(b => b.$btn),
        });
    }
};