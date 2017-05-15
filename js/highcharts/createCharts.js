
/**
 * MISS Statistics UI - Highcharts Implementation (http://highcharts.com/)
 *
 * @file createCharts.js
 * @version 0.1
 */

jQuery(document).ready(function($) {

  if(Drupal.settings.miss.multiFile) {

    $.each(Drupal.settings.miss.statsData, function(skey, svalue) {

      switch(skey) {
        case '1968':
        case 'journey1968':
          keyLabel = 'Beyond_1968';
          break;
        case 'eos':
        case 'journeyEos':
          keyLabel = 'The_End_of_Segregation';
          break;
        case 'sf':
        case 'journeySf':
          keyLabel = 'Slavery_&_Freedom';
          break;
        default:
          keyLabel = skey;
      }

      Highcharts.chart(keyLabel, {
          chart: {
              zoomType: 'x'
          },
          plotOptions: {
              line: {
                  marker: {
                      enabled: false
                  }
              }
          },
          title: {
              text: keyLabel.replace(/_/g, ' ')
          },
          subtitle: {
              text: document.ontouchstart === undefined ?
              'Click and drag in the plot area to zoom in' : 'Pinch the chart to zoom in'
          },
          xAxis: {
              type: 'datetime',
              categories: svalue.categories
          },
          yAxis: {
              title: {
                  text: 'Total Sessions'
              }
          },
          legend: {
              enabled: false
          },
          series: [{
              name: 'Totals',
              data: svalue.totals
          }],
          credits: {
            enabled: false
          }
      });

    });

  }

  if(!Drupal.settings.miss.multiFile) {

    Highcharts.chart('total', {
        chart: {
            zoomType: 'x'
        },
        plotOptions: {
            line: {
                marker: {
                    enabled: false
                }
            }
        },
        title: {
            text: Drupal.settings.miss.interactiveName + ' Stats'
        },
        subtitle: {
            text: document.ontouchstart === undefined ?
            'Click and drag in the plot area to zoom in' : 'Pinch the chart to zoom in'
        },
        xAxis: {
            type: 'datetime',
            categories: Drupal.settings.miss.statsData.categories
        },
        yAxis: {
            title: {
                text: 'Number of Uploads'
            }
        },
        legend: {
            enabled: false
        },
        series: [{
            name: 'Totals',
            data: Drupal.settings.miss.statsData.totals
        }],
        credits: {
          enabled: false
        }
    });

  }


});
