import InputWidget from "./input";

export default class PassWidget extends InputWidget {
    constructor(data) {
        super(data);
        this.$out.style.filter = 'blur(4px)';
    }
}