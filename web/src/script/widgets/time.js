import { Component } from "@alexgyver/component";
import WidgetBase from "./widget";

class TimeWidgetBase extends WidgetBase {
    constructor(data, type) {
        super(data);
        this.type = type;

        super.addOutput(Component.make('div', {
            context: this,
            class: 'input_cont',
            children: [
                {
                    tag: 'input',
                    var: 'input',
                    type: this.type,
                    class: 'date',
                    step: 1,
                    events: {
                        change: () => {
                            let unix = Math.floor(this.$input.valueAsNumber / 1000);
                            this.sendEvent(unix);
                            this.update(unix);
                        }
                    }
                },
                {
                    tag: 'span',
                    class: 'value active',
                    var: 'out',
                    events: {
                        click: () => this.$input.showPicker(),
                    },
                }
            ]
        }));

        this.update(data.value);
    }

    update(value) {
        value = value ?? 0;
        this.$input.value = this.unixToDate(value);
        this.$out.innerText = this.unixToText(value);
    }

    unixToDate(value) {
        value = new Date(value * 1000).toISOString();
        switch (this.type) {
            case 'date': return value.split('T')[0];
            case 'time': return value.split('T')[1].split('.')[0];
            case 'datetime-local': return value.split('.')[0];
        }
    }
    unixToText(value) {
        return this.unixToDate(value).replace('T', ' ');
    }
}

export class TimeWidget extends TimeWidgetBase {
    constructor(data) {
        super(data, 'time');
    }
}

export class DateWidget extends TimeWidgetBase {
    constructor(data) {
        super(data, 'date');
    }
}

export class DateTimeWidget extends TimeWidgetBase {
    constructor(data) {
        super(data, 'datetime-local');
    }
}