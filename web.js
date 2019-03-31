var express = require('express');
var app = express();

mysql = require('mysql');
fs = require('fs');

var array=fs.readFileSync('id.txt').toString().split("\n");


var connection = mysql.createConnection({

    host: array[0],

    user: array[1],

    password: array[2],

    database: array[3]

})

connection.connect();



function insert_sensor(temp) {

  obj={};
  obj.temp=temp;



  var query = connection.query('insert into myroom set ?', obj, function(err, rows, cols) {

    if (err) throw err;

    console.log("database insertion ok= %j", obj);

  });

}



app.get('/', function(req, res) {

  res.end('Nice to meet you');

});



app.get('/log', function(req, res) {

  r = req.query;

  console.log("GET %j", r);
  insert_sensor(r.temp);

  res.end('OK:' + JSON.stringify(req.query));

});

app.get('/graph', function (req, res) {

    console.log('got app.get(graph)');

    var html = fs.readFile('./graph.html', function (err, html) {

    html = " "+ html

    console.log('read file');



    var qstr = 'select * from myroom ';

    connection.query(qstr, function(err, rows, cols) {

      if (err) throw err;



      var data = "";

      var comma = ""

      for (var i=0; i< rows.length; i++) {

         r = rows[i];

         data += comma + "[new Date('"+ r.time +"'),"+ r.temp +"]";

         comma = ",";

      }

      var header = "data.addColumn('date', 'Date/Time');"

      header += "data.addColumn('number', 'Temp');"

      html = html.replace("<%HEADER%>", header);
      html = html.replace("<%DATA%>", data);
      html = html.replace("<%STIME%>", rows[0].time);
      html = html.replace("<%ETIME%>", rows[rows.length-1].time); res.writeHeader(200, {"Content-Type": "text/html"});

      res.write(html);

      res.end();

    });

  });

})





var server = app.listen(9000, function () {

  var host = 'ec2-52-12-68-77.us-west-2.compute.amazonaws.com'

  var port = 9000

  console.log('listening at http://%s:%s', host, port)

});


