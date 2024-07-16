import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";

export default class ParagraphWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addChild(Component.make('p', {
            context: this,
            var: 'out',
            style: {
                margin: '0',
                padding: '0 12px 12px 12px',
            }
        }));

        this.update(data.value);
    }

    update(value) {
        this.$out.innerText = value ?? '';
    }
}