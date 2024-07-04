import { Component } from "@alexgyver/component";

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
        this.$root.dispatchEvent(new CustomEvent("set", {
            bubbles: true,
            detail: {
                id: this.id,
                value: value,
            }
        }));
    }

    update(value) { }
}