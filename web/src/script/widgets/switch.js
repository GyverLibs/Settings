import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";

export default class SwitchWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('input', {
            context: this,
            class: 'switch',
            type: 'checkbox',
            var: 'switch',
            events: {
                click: () => this.sendEvent(this.$switch.checked ? 1 : 0),
            }
        }));

        this.update(data.value);
    }

    update(value) {
        this.$switch.checked = Number(value ?? 0);
    }
}