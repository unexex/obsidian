"use strict";

const webpack = require('webpack');

module.exports = [
	{
		name: 'script-tag',
		entry: './src/web.ts', // change this to your TypeScript entry file
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
			  test: [/\.tsx?$/], // change this to handle .ts and .tsx files
			  use: 'ts-loader',
			  exclude: /node_modules/,
			},
			// other rules...
		  ],
		},
		resolve: {
		  extensions: ['.tsx', '.ts', '.js'], // add .ts and .tsx here
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
		entry: './src/web.ts',
		target: 'web',
		output: {
			filename: 'web.bundle.js',
			libraryTarget: 'commonjs2'
		},
		devtool: 'hidden-source-map',
		node: false,
		resolve: {
		  extensions: ['.tsx', '.ts', '.js'], // add .ts and .tsx here
		},
		plugins: [
			new webpack.DefinePlugin({
				"process.env.FENGARICONF": "void 0",
				"typeof process": JSON.stringify("undefined")
			})
		]
	}
];