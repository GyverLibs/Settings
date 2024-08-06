import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";
import './led.css';

export default class LedWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('span', {
            context: this,
            class: 'led',
            var: 'led',
        }));

        this.update(data.value);
    }

    update(value) {
        this.$led.classList = 'led' + (value == '1' ? ' on' : '');
    }
}