import { Component } from "@alexgyver/component";
import { Arrow } from "./misc";
import WidgetBase from "./widget";

export default class SelectWidget extends WidgetBase {
    constructor(data) {
        super(data);
        this.text = data.text;
        data.value = data.value ?? 0;

        super.addOutput(Component.make('div', {
            context: this,
            class: 'input_cont',
            children: [
                {
                    tag: 'select',
                    context: this,
                    var: 'sel',
                    class: 'select',
                    children: data.text.split(';').map((x, i) => Component.make('option', { value: i, text: x, selected: (i == data.value) })),
                    events: {
                        change: () => {
                            this.sendEvent(this.$sel.value);
                            this.update(this.$sel.value);
                        },
                    }
                },
                {
                    tag: 'div',
                    class: 'input_arrow',
                    events: {
                        click: () => this.$sel.showPicker(),
                    },
                    children: [
                        {
                            tag: 'span',
                            class: 'value active',
                            style: 'padding-right: 8px',
                            var: 'out',
                        },
                        Arrow('down', 15),
                    ]
                }
            ]
        }));

        this.update(data.value);
    }

    update(value) {
        this.$sel.value = value;
        this.$out.innerText = this.text.split(';')[value];
    }
}