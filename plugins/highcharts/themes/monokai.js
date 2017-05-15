
/**
 * @license Highcharts JS v5.0.6 (2016-12-07)
 *
 * (c) 2009-2016 Torstein Honsi
 *
 * License: www.highcharts.com/license
 */
(function(factory) {
    if (typeof module === 'object' && module.exports) {
        module.exports = factory;
    } else {
        factory(Highcharts);
    }
}(function(Highcharts) {
    (function(Highcharts) {
        /**
         * (c) 2010-2016 Torstein Honsi
         *
         * License: www.highcharts.com/license
         * 
         * Grid-light theme for Highcharts JS
         * @author Torstein Honsi
         */

        'use strict';
        /* global document */
        // Load the fonts
        Highcharts.createElement('link', {
            href: 'https://fonts.googleapis.com/css?family=Dosis:400,600',
            rel: 'stylesheet',
            type: 'text/css'
        }, null, document.getElementsByTagName('head')[0]);

        Highcharts.theme = {
  "colors": ["#F92672", "#66D9EF", "#A6E22E", "#A6E22E"],
  "chart": {
    "backgroundColor": "#272822",
    "style": {
      "fontFamily": "Inconsolata",
      "color": "#A2A39C"
    }
  },
  "title": {
    "style": {
      "color": "#A2A39C"
    },
    "align": "left"
  },
  "subtitle": {
    "style": {
      "color": "#A2A39C"
    },
    "align": "left"
  },
  "legend": {
    "align": "right",
    "verticalAlign": "bottom",
    "itemStyle": {
      "fontWeight": "normal",
      "color": "#A2A39C"
    }
  },
  "xAxis": {
    "gridLineDashStyle": "Dot",
    "gridLineWidth": 1,
    "gridLineColor": "#A2A39C",
    "lineColor": "#A2A39C",
    "minorGridLineColor": "#A2A39C",
    "tickColor": "#A2A39C",
    "tickWidth": 1
  },
  "yAxis": {
    "gridLineDashStyle": "Dot",
    "gridLineColor": "#A2A39C",
    "lineColor": "#A2A39C",
    "minorGridLineColor": "#A2A39C",
    "tickColor": "#A2A39C",
    "tickWidth": 1
  }
};

        // Apply the theme
        Highcharts.setOptions(Highcharts.theme);

    }(Highcharts));
}));
