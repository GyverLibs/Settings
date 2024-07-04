import InputWidget from "./input";

export default class PassWidget extends InputWidget {
    constructor(data) {
        super(data);
        this.$input.style.filter = 'blur(4px)';
    }
}