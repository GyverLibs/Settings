import { Component } from "@alexgyver/component";
import { Arrow } from "./misc";
import WidgetBase from "./widget";

export default class SelectWidget extends WidgetBase {
    constructor(data) {
        super(data);
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
                    children: data.text.split(';').map((x, i) => Component.make('option', { value: i.toString(), text: x, selected: (i == data.value) })),
                    events: {
                        change: () => {
                            this.sendEvent(this.$sel.value);
                            this.update(this.$sel.value);
                        },
                    }
                },
            ]
        }));
    }

    update(value) {
        this.$sel.value = value ?? 0;
    }
}