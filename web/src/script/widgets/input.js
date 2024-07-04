import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";

export default class InputWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('span', {
            context: this,
            class: 'value active',
            var: 'input',
            events: {
                click: () => {
                    let res = prompt(undefined, this.$input.innerText);
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
        this.$input.innerText = value;
    }
}