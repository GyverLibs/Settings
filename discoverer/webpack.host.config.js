const path = require('path');
const webpack = require("webpack");
var PACKAGE = require('./package.json');

const ReplaceHashInFileWebpackPlugin = require('replace-hash-in-file-webpack-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const TerserPlugin = require("terser-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const CopyPlugin = require("copy-webpack-plugin");

module.exports = {
    entry: {
        index: './src/index.js',
    },

    output: {
        filename: 'script.js',
        path: path.resolve(__dirname, 'dist/host'),
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
            },
        ]
    },

    plugins: [
        new HtmlWebpackPlugin({
            template: `./src/index.html`,
            filename: `index.html`,
            favicon: "./src/assets/favicon.svg",
            inject: true,
            minify: false,
            hash: true,
            version: PACKAGE.version,
            title: PACKAGE.title,
            manifest: '<link rel="manifest" href="manifest.json" />',
        }),
        new MiniCssExtractPlugin({
            filename: 'style.css',
        }),
        new webpack.DefinePlugin({ USE_SW: JSON.stringify(true) }),
        new CopyPlugin({
            patterns: [
                { from: "src/assets/sw.js", to: "" },
                { from: "src/assets/manifest.json", to: "" },
                { from: "src/assets/icon.png", to: "" },
            ],
        }),
        new ReplaceHashInFileWebpackPlugin([
            {
                dir: 'dist/host',
                files: ['sw.js'],
                rules: [
                    {
                        search: /@cachename/,
                        replace: '[hash]'
                    }
                ]
            },
            {
                dir: 'dist/host',
                files: ['manifest.json'],
                rules: [
                    {
                        search: /@theme_color/,
                        replace: PACKAGE.theme_color,
                    },
                    {
                        search: /@background_color/,
                        replace: PACKAGE.background_color,
                    },
                    {
                        search: /@name/g,
                        replace: PACKAGE.title,
                    },
                    {
                        search: /@descr/,
                        replace: PACKAGE.descr,
                    },
                ]
            }
        ])
    ],

    mode: 'production',
};