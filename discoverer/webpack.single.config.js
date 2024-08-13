const path = require('path');
var PACKAGE = require('./package.json');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const TerserPlugin = require("terser-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const HTMLInlineCSSWebpackPlugin = require("html-inline-css-webpack-plugin").default;
const HtmlInlineScriptPlugin = require('html-inline-script-webpack-plugin');

module.exports = {
    entry: {
        index: './src/index.js',
    },

    output: {
        filename: 'script.js',
        path: path.resolve(__dirname, 'dist/single'),
        clean: true,
        publicPath: '',
    },

    optimization: {
        minimize: true,
        minimizer: [
            new CssMinimizerPlugin(),
            new TerserPlugin(),
        ],
    },

    module: {
        rules: [
            {
                test: /\.css$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    "css-loader"
                ]
            }
        ]
    },

    plugins: [
        new HtmlWebpackPlugin({
            template: `./src/index.html`,
            filename: `index.html`,
            inject: true,
            minify: false,
            version: PACKAGE.version,
        }),
        new HtmlInlineScriptPlugin({
            htmlMatchPattern: [/index.html$/],
        }),
        new MiniCssExtractPlugin({
            filename: 'style.css',
        }),
        new HTMLInlineCSSWebpackPlugin(),
    ],

    mode: 'production',
};