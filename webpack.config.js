"use strict";

const webpack = require('webpack');

module.exports = [
	{
		/*
		This target produces a file suitable for embedding onto any webpage via a <script> tag.
		*/
		name: 'script-tag',
		entry: './src/web.js',
		target: 'web',
		output: {
			filename: 'web.js',
			library: 'web',
			libraryTarget: 'umd'
		},
		devtool: 'hidden-source-map',
		node: false,
		module: {
			rules: [
				{
					test: [/\.js$/],
					loader: 'babel-loader',
					options: {
						presets: [['@babel/preset-env', {
							"targets": {
								"browsers": ["last 2 versions", "not safari <= 7", "not ie <= 10"]
							}
						}]]
					}
				}
			]
		},
		plugins: [
			new webpack.DefinePlugin({
				"process.env.FENGARICONF": "void 0",
				"typeof process": JSON.stringify("undefined")
			})
		]
	},
	{
		/*
		This target exists to create a bundle that has the node-specific paths eliminated.
		It is expected that most people would minify this with their own build process
		*/
		name: 'bundle',
		entry: './src/web.js',
		target: 'web',
		output: {
			filename: 'web.bundle.js',
			libraryTarget: 'commonjs2'
		},
		devtool: 'hidden-source-map',
		node: false,
		plugins: [
			new webpack.DefinePlugin({
				"process.env.FENGARICONF": "void 0",
				"typeof process": JSON.stringify("undefined")
			})
		]
	}
];