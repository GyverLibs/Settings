import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";
import { AsyncPrompt } from "../dialog";

export default class InputWidget extends WidgetBase {
    empty = false;
    constructor(data) {
        super(data);

        super.addOutput(Component.make('span', {
            context: this,
            class: 'value active',
            var: 'input',
            events: {
                click: async () => {
                    let res = await AsyncPrompt(data.label, this.empty ? '' : this.$input.innerText);
                    if (res != null) {
                        this.sendEvent(res);
                        this.update(res);
                    }
                }
            }
        }));

        this.update(data.value);
    }

    update(value) {
        value = value ? value.toString() : '';
        this.empty = (value.length == 0);
        this.$input.innerText = this.empty ? '...' : value;
    }
}