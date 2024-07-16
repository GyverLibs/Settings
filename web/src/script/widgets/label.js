import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";
import { intToColor } from "./misc";

export default class LabelWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('span', {
            context: this,
            var: 'out',
            class: 'value bold',
            style: {
                color: data.color ? intToColor(data.color) : 'var(--font_tint)',
            }
        }));

        this.update(data.value);
    }

    update(value) {
        this.$out.innerText = value ?? '';
    }
}