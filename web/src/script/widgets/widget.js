import { Component } from "@alexgyver/component";
import WidgetEvent from "./event";

export default class WidgetBase {
    $root;
    $error;

    constructor(data) {
        this.id = data.id;
        Component.make('div', {
            context: this,
            class: 'widget',
            var: 'root',
            child: {
                tag: 'div',
                class: 'widget_row',
                var: 'row',
                children: [
                    {
                        tag: 'div',
                        style: 'padding-right: 8px',
                        children: [
                            {
                                tag: 'label',
                                text: data.label,
                            },
                            {
                                tag: 'sup',
                                class: 'error_sup',
                                text: 'Error!',
                                var: 'error',
                            }
                        ]
                    }
                ]
            }
        });
    }

    addOutput(out) {
        this.$row.append(out);
    }

    addChild(child) {
        this.$root.append(child);
    }

    sendEvent(value) {
        this.$root.dispatchEvent(new WidgetEvent('set', this.id, value, this));
    }

    setError() {
        this.$error.style.display = 'inline';
        setTimeout(() => {
            if (this.$error) this.$error.style.display = 'none';
        }, 2500);
    }

    update(value) { }
}