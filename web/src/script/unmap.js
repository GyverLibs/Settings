export default class unMap extends Map {
    count = 0;

    constructor() {
        super();
    }
    set(id, value) {
        if (id === undefined) id = 'undef_' + (this.count++);
        super.set(id, value);
    }
}