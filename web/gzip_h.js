const pkg = require('./package.json');
const in_folder = './dist/index';
const out_folder = './dist/gzip';

const fs = require('node:fs');
const { promisify } = require('node:util');
const { createGzip } = require('node:zlib');
const { pipeline } = require('node:stream');
const pipe = promisify(pipeline);

async function gzip(input, output) {
    const gzip = createGzip();
    const source = fs.createReadStream(input);
    const destination = fs.createWriteStream(output);
    await pipe(source, gzip, destination);
}

async function compile() {
    const index_gz = out_folder + '/index.html.gz';
    const script_gz = out_folder + '/script.js.gz';
    const style_gz = out_folder + '/style.css.gz';

    try {
        if (!fs.existsSync(out_folder)) fs.mkdirSync(out_folder);
        await gzip(in_folder + '/index.html', index_gz);
        await gzip(in_folder + '/script.js', script_gz);
        await gzip(in_folder + '/style.css', style_gz);
    } catch (err) {
        console.error(err);
        return;
    }

    let index_len = fs.statSync(index_gz).size;
    let script_len = fs.statSync(script_gz).size;
    let style_len = fs.statSync(style_gz).size;

    let code = `#pragma once
#include <Arduino.h>

/*
    ${pkg.name}.h v${pkg.version}
    index: ${index_len} bytes
    script: ${script_len} bytes
    style: ${style_len} bytes
    total: ${((index_len + script_len + style_len) / 1024).toFixed(2)} kB
    
    Build: ${new Date()}
*/
`;

    function addBin(fname, gzip) {
        let data = fs.readFileSync(gzip).toString('hex');
        let code = '\r\n' + `const uint8_t ${pkg.name}_${fname}[] PROGMEM = {`;
        for (let i = 0; i < data.length; i += 2) {
            if (i % 48 == 0) code += '\r\n    ';
            code += '0x' + data[i] + data[i + 1];
            if (i < data.length - 2) code += ', ';
        }
        code += '\r\n};\r\n'
        code += `const size_t ${pkg.name}_${fname}_len = ${data.length / 2};`;
        code += '\r\n'
        return code;
    }

    code += addBin('index_gz', index_gz);
    code += addBin('script_gz', script_gz);
    code += addBin('style_gz', style_gz);

    fs.writeFile(`${out_folder}/${pkg.name}.h`, code, err => {
        if (err) console.error(err);
        else console.log('Done! Gzipped to ' + ((index_len + script_len + style_len) / 1024).toFixed(2) + ' kB');
    });
}

compile();