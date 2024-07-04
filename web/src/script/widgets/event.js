export default class WidgetEvent extends Event {
    constructor(action, id, value = null, widget = null) {
        super("widget_event", { bubbles: true });
        this.data = {
            action: action,
            id: id.toString(16),
            value: value,
            widget: widget,
        };
    }
}