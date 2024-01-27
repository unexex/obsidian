"use strict";

const webpack = require('webpack');
const TerserPlugin = require('terser-webpack-plugin');

module.exports = [
	{
		name: 'script-tag',
		optimization: {
			minimizer: [
			  new TerserPlugin({
				terserOptions: {
				  keep_fnames: true,
				},
			  }),
			],
		  },
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
		  ],
		},
		resolve: {
		  extensions: ['.js'],
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
		optimization: {
			minimizer: [
			  new TerserPlugin({
				terserOptions: {
				  keep_fnames: true
				},
			  }),
			],
		  },
		entry: './src/web.js',
		target: 'web',
		output: {
			filename: 'web.bundle.js',
			library: 'ob',
			libraryTarget: 'var' // or windows
		},
		devtool: 'hidden-source-map',
		node: false,
		resolve: {
		  extensions: ['.js'], // add .ts and .tsx here
		},
		plugins: [
			new webpack.DefinePlugin({
				"process.env.FENGARICONF": "void 0",
				"typeof process": JSON.stringify("undefined")
			})
		]
	}
];