import { AsyncConfirm } from "../ui/dialog";
import WidgetBase from "./widget";

export default class ConfirmWidget extends WidgetBase {
    constructor(data) {
        super(data, false);
        this.text = data.label ?? 'Confirm';
        this.$root = document.createElement('div');
    }

    async update(value) {
        let res = await AsyncConfirm(value ?? this.text);
        this.sendEvent(res ? 1 : 0);
    }
}