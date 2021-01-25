'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

app.get('/index.js', function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.js'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});



//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

var lib = ffi.Library('./libsvgparse', {
    'fileToImageToJson': [ 'string', [ 'string', 'string' ] ],
    'title' : [ 'string', [ 'string', 'string' ] ],
    'desc' : [ 'string', [ 'string', 'string' ] ],
    'circJSON': [ 'string', [ 'string', 'string' ] ],
    'rectJSON': [ 'string', [ 'string', 'string' ] ],
    'pathJSON': [ 'string', [ 'string', 'string' ] ],
    'groupJSON': [ 'string', [ 'string', 'string' ] ],
    'getAttributes' : ['string', [ 'string', 'string', 'string'] ],
    'createSVGFileFromJSON' : ['bool', ['string', 'string', 'string']],
    'addRectToSVG' : ['bool', ['string', 'string', 'string']],
    'addCircleToSVG' : ['bool', ['string', 'string', 'string']],
    'addAttrToSVG' : ['bool', ['string', 'string', 'int', 'string']]
});

app.get('/getimages', function(req , res){
  var jsonArr = [];

  fs.readdirSync("./uploads/").forEach(file => {    //gets files and number of elements
    var svg = JSON.parse(lib.fileToImageToJson("./uploads/" + file, "./parser/svg.xsd"));
    if(svg != "{}") {
        var image = {
        fileName : file,
        fileSize : getFilesizeInBytes("./uploads/" + file),
        numrects : svg.numRect,
        numcircles: svg.numCirc,
        numpaths: svg.numPaths,
        numgroups: svg.numGroups,
      };      
      jsonArr.push(image);
    }
  });
  res.send(jsonArr);
});

app.get('/getfullimages', function(req , res){    //gets images, shapes and all attributes
  var jsonArr = [];

  fs.readdirSync("./uploads/").forEach(file => {
    var svg = JSON.parse(lib.fileToImageToJson("./uploads/" + file, "./parser/svg.xsd"));
    if(svg != "{}") {
        var image = {
        fileName : file,
        fileSize : getFilesizeInBytes("./uploads/" + file),
        numrects : svg.numRect,
        numcircles: svg.numCirc,
        numpaths: svg.numPaths,
        numgroups: svg.numGroups,
        title: lib.title("./uploads/" + file, "./parser/svg.xsd"),
        description: lib.desc("./uploads/" + file, "./parser/svg.xsd"),
        rects: lib.rectJSON("./uploads/" + file, "./parser/svg.xsd"),
        circs: lib.circJSON("./uploads/" + file, "./parser/svg.xsd"),
        paths: lib.pathJSON("./uploads/" + file, "./parser/svg.xsd"),
        groups: lib.groupJSON("./uploads/" + file, "./parser/svg.xsd"),
        rectAttr: JSON.parse(lib.getAttributes("./uploads/" + file, "rectangle", "./parser/svg.xsd")),
        circAttr: JSON.parse(lib.getAttributes("./uploads/" + file, "circle", "./parser/svg.xsd")),
        pathAttr: JSON.parse(lib.getAttributes("./uploads/" + file, "path", "./parser/svg.xsd")),
        groupAttr: JSON.parse(lib.getAttributes("./uploads/" + file, "group", "./parser/svg.xsd"))
      };      
      jsonArr.push(image);
    }
  });
  res.send(jsonArr);
});

app.post('/sendattr', function(req,res){    //send attribute to server
  var json = {};
    req.on('data', function(data) {
      data = data.toString();
      data = data.split('&'); 
      
      for(let i = 0; i < data.length; i++) {
        var data2 = data[i].split("=");
        json[data2[0]] = data2[1];
      }
      let name = "./uploads/" + json.filename;
      delete json.filename;
      let row = json.row;
      delete json.row;
      let val = lib.addAttrToSVG(JSON.stringify(json),name,row,"./parser/svg.xsd");
  })
    res.redirect('/');
});

app.post('/sendfile', function(req,res){      //send file to server
  let json = {};
    req.on('data', function(data) {
      data = data.toString();
      data = data.split('&'); 
      
      for(let i = 0; i < data.length; i++) {
        var data2 = data[i].split("=");
        json[data2[0]] = data2[1];
      }
      let name = json.filename;
      delete json.filename;
      let val = lib.createSVGFileFromJSON(JSON.stringify(json),name,"./parser/svg.xsd");
    })
    res.redirect('/');
});

app.post('/sendrect', function(req,res){    //send rectangle to server
  let json = {};
    req.on('data', function(data) {
      data = data.toString();
      data = data.split('&'); 
      
      for(let i = 0; i < data.length; i++) {
        var data2 = data[i].split("=");
        json[data2[0]] = data2[1];
      }
      let name = "./uploads/" + json.filename;
      delete json.filename;
      let val = lib.addRectToSVG(JSON.stringify(json),name,"./parser/svg.xsd");
    })
    res.redirect('/');
});

app.post('/sendcircle', function(req,res){    //send circle to server
  let json = {};
    req.on('data', function(data) {
      data = data.toString();
      data = data.split('&'); 
      
      for(let i = 0; i < data.length; i++) {
        var data2 = data[i].split("=");
        json[data2[0]] = data2[1];
      }
      let name = "./uploads/" + json.filename;
      delete json.filename;
      let val = lib.addCircleToSVG(JSON.stringify(json),name,"./parser/svg.xsd");
    })
    res.redirect('/');
});

function getFilesizeInBytes(filename) {
  var stats = fs.statSync(filename);
  return stats["size"] / 1000;
}

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);







