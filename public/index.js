// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    /*Creates console log panel*/
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getimages',   //The server endpoint we are connecting to
        
        success: function(data) {
            listView(data);         //makes console log panel 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#filename').html("On page load, received error from server");
            console.log(error); 
        }
    });

    /*Creates dropdown list and adds listener*/
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getfullimages',   //The server endpoint we are connecting to
        success: function (data) {
            let list = document.getElementById('images');
            for (let i = 0; i < data.length; i++) {
                let option = document.createElement('option');
                option.innerHTML = data[i].fileName
                list.appendChild(option);
            }

            list.addEventListener('change', function(){svgView(data)},true)     //file selector listener
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    /*Console log panel*/
    function listView(data) {
        let table = document.getElementById('fileTable');
        document.getElementById('div0').style.border = "2px solid #ffffff";
        document.getElementById('div0').style.height = "500px";
        for (let i = 0; i < data.length; i++) {     //dynamically display stuff for each file
            let row = table.insertRow(table.rows.length);
            
            let fnCell = row.insertCell(0);             //photo
            let link = document.createElement('a');
            link.href = data[i].fileName;
            link.download = data[i].fileName;
            let element = document.createElement('img');
            element.src = data[i].fileName;
            element.width = 150;
            element.height = 150;
            link.appendChild(element);
            fnCell.appendChild(link);

            fnCell = row.insertCell(1);                 //file name
            element = document.createElement('a');
            let text = document.createTextNode(data[i].fileName);
            element.href = data[i].fileName;
            element.download = data[i].fileName;
            element.appendChild(text);
            fnCell.appendChild(element);

            fnCell = row.insertCell(2);                 //file size
            element = document.createElement('p');
            text = document.createTextNode(data[i].fileSize + "KB");
            element.appendChild(text);
            fnCell.appendChild(element);

            fnCell = row.insertCell(3);                 //rectangles
            element = document.createElement('p');
            text = document.createTextNode(data[i].numrects);
            element.appendChild(text);
            fnCell.appendChild(element);

            fnCell = row.insertCell(4);                 //circles
            element = document.createElement('p');
            text = document.createTextNode(data[i].numcircles);
            element.appendChild(text);
            fnCell.appendChild(element);

            fnCell = row.insertCell(5);                 //paths
            element = document.createElement('p');
            text = document.createTextNode(data[i].numpaths);
            element.appendChild(text);
            fnCell.appendChild(element);
            
            fnCell = row.insertCell(6);                 //groups
            element = document.createElement('p');
            text = document.createTextNode(data[i].numgroups);
            element.appendChild(text);
            fnCell.appendChild(element);
        }
        var addBtn = document.getElementById('addImage');
        addBtn.addEventListener('click', function(){
            addImage(data);
        });
    }
    
    /*File view panel*/
    function svgView(data) {
        document.getElementById('div1').style.border = "2px solid #ffffff";
        document.getElementById('div1').style.height = "700px";
        var selectBox = document.getElementById('images');
        var selected = selectBox.options[selectBox.selectedIndex].value;
        let t1 = document.getElementById('t1');
        let t2 = document.getElementById('t2');
        let t3 = document.getElementById('t3');
        let t4 = document.getElementById('t4');
        let t5 = document.getElementById('t5');
        if(t4.rows.length > 1) {    //delete rows
            for(let i = t4.rows.length - 1; i >= 1; i--) {
                t4.deleteRow(i);
            }
        }

        if(t5.rows.length > 1) {    //delete rows
            for(let i = t5.rows.length - 1; i >= 1; i--) {
                t5.deleteRow(i);
            }
        }
        
        for (let i = 0; i < data.length; i++) { /*Display all elements in the file*/
            if(selected == data[i].fileName) {
                if(t1.rows.length == 2) {
                    t1.deleteRow(1);
                }
                let row = t1.insertRow(1);
                let fnCell = row.insertCell(0);
                let link = document.createElement('a');
                link.href = data[i].fileName;
                link.download = data[i].fileName;
                let element = document.createElement('img');
                element.src = data[i].fileName;
                element.width = 300;
                element.height = 300;
                link.appendChild(element);
                fnCell.appendChild(link);

                if(t2.rows.length == 2) {
                    t2.deleteRow(1);
                }
                row = t2.insertRow(1);
                fnCell = row.insertCell(0);
                element = document.createElement('p');
                let text = document.createTextNode(data[i].title);
                element.appendChild(text);
                fnCell.appendChild(element);

                fnCell = row.insertCell(1);
                element = document.createElement('p');
                text = document.createTextNode(data[i].description);
                element.appendChild(text);
                fnCell.appendChild(element);

                if(t3.rows.length > 1) {
                    for(let j = t3.rows.length - 1; j >= 1; j--) {
                        t3.deleteRow(j);
                    }
                }
                
                var groups = JSON.parse(data[i].groups);
                
                for(let j = 0; j < data[i].numgroups; j++) {
                    row = t3.insertRow(j + 1);
                    fnCell = row.insertCell(0);
                    element = document.createElement('p');
                    text = document.createTextNode("Group " + (j + 1));
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(1);
                    element = document.createElement('p');
                    text = document.createTextNode("Child elements: " + groups[j].children);
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(2);
                    element = document.createElement('p');
                    text = document.createTextNode(groups[j].numAttr);
                    element.appendChild(text);
                    fnCell.appendChild(element);
                }

                var paths = JSON.parse(data[i].paths);
                
                for(let j = 0; j < data[i].numpaths; j++) {
                    row = t3.insertRow(j + 1);
                    fnCell = row.insertCell(0);
                    element = document.createElement('p');
                    text = document.createTextNode("Path " + (j + 1));
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(1);
                    element = document.createElement('p');
                    text = document.createTextNode("Data: " + paths[j].d);
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(2);
                    element = document.createElement('p');
                    text = document.createTextNode(paths[j].numAttr);
                    element.appendChild(text);
                    fnCell.appendChild(element);
                }

                var circs = JSON.parse(data[i].circs);
                
                for(let j = 0; j < data[i].numcircles; j++) {
                    row = t3.insertRow(j + 1);
                    fnCell = row.insertCell(0);
                    element = document.createElement('p');
                    text = document.createTextNode("Circle " + (j + 1));
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(1);
                    element = document.createElement('p');
                    text = document.createTextNode("CX: " + circs[j].cx);
                    element.appendChild(text);
                    text = document.createElement('br');
                    element.appendChild(text);
                    text = document.createTextNode("CY: " + circs[j].cy);
                    element.appendChild(text);
                    text = document.createElement('br');
                    element.appendChild(text);
                    text = document.createTextNode("Radius: " + circs[j].r + circs[j].units);
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(2);
                    element = document.createElement('p');
                    text = document.createTextNode(circs[j].numAttr);
                    element.appendChild(text);
                    fnCell.appendChild(element);
                }
                
                var rects = JSON.parse(data[i].rects);
                
                for(let j = 0; j < data[i].numrects; j++) {
                    row = t3.insertRow(j + 1);
                    fnCell = row.insertCell(0);
                    element = document.createElement('p');
                    text = document.createTextNode("Rectangle " + (j + 1));
                    element.appendChild(text);
                    fnCell.appendChild(element);

                    fnCell = row.insertCell(1);
                    element = document.createElement('p');
                    text = document.createTextNode("X: " + rects[j].x);
                    element.appendChild(text);
                    text = document.createElement('br');
                    element.appendChild(text);
                    text = document.createTextNode("Y: " + rects[j].y);
                    element.appendChild(text);
                    text = document.createElement('br');
                    element.appendChild(text);
                    text = document.createTextNode("Width: " + rects[j].w + rects[j].units);
                    element.appendChild(text);
                    text = document.createElement('br');
                    element.appendChild(text);
                    text = document.createTextNode("Height: " + rects[j].h + rects[j].units);
                    element.appendChild(text);
                    fnCell.appendChild(element);
                    
                    fnCell = row.insertCell(2);
                    element = document.createElement('p');
                    text = document.createTextNode(rects[j].numAttr);
                    element.appendChild(text);
                    fnCell.appendChild(element);
                }
                break;
            } 
        }

        var cells = t3.getElementsByTagName('td');  /*Add listener to each cell for attribute view panel*/
        for (var i = 0; i < cells.length; i++) {
            var cell = cells[i];

            cell.onclick =  function(){
                var rowId = this.parentNode.rowIndex;
                attrView(data,rowId,selected)};
        }

        /*Buttons for adding new elements*/
        var addCBtn = document.getElementById('addCircle'); 
        addCBtn.addEventListener('click', function(){
            addCircle(selected);
        });

        var addRBtn = document.getElementById('addRect');
        addRBtn.addEventListener('click', function(){
            addRect(data,selected);
        });
    }

    /*Attribute view panel*/
    function attrView(data,rowId,selected){
        let t3 = document.getElementById('t3');
        let t4 = document.getElementById('t4');
        let t5 = document.getElementById('t5');
        var rowsNotSelected = t3.getElementsByTagName('tr');

        /*Highlight the row in the previous table to indicat which component is being displayed*/
        for (let i = 0; i < rowsNotSelected.length; i++) {
            rowsNotSelected[i].style.backgroundColor = "";
        }

        if(t4.rows.length > 1) {    //delete rows
            for(let i = t4.rows.length - 1; i >= 1; i--) {
                t4.deleteRow(i);
            }
        }

        if(t5.rows.length > 1) {    //delete rows
            for(let i = t5.rows.length - 1; i >= 1; i--) {
                t5.deleteRow(i);
            }
        }

        var rowSelected = t3.getElementsByTagName('tr')[rowId];
        rowSelected.style.backgroundColor = '#f1d2a298';

        var addBtn = document.getElementById('addAttr');    //add attribute button
        addBtn.addEventListener('click', function(){
            addAttr(data,rowId,selected);
        });

        /*Display attributes*/
        for (let i = 0; i < data.length; i++) {
            var rects = JSON.parse(data[i].rects);
            var circs = JSON.parse(data[i].circs);
            var paths = JSON.parse(data[i].paths);
            var groups = JSON.parse(data[i].groups);
            if(selected == data[i].fileName) {
                let count = 1;
                for(let j = 0; j < data[i].numrects; j++) {
                    if(count == rowId) {
                        row = t4.insertRow(1);
                        fnCell = row.insertCell(0);
                        element = document.createElement('p');
                        text = document.createTextNode("Rectangle " + (j + 1));
                        element.appendChild(text);
                        fnCell.appendChild(element);
                                                               
                        for(let k = 0; k < rects[j].numAttr; k++) {
                            row = t5.insertRow(k + 1);
                            fnCell = row.insertCell(0);
                            element = document.createElement('p');
                            text = document.createTextNode((k + 1));
                            element.appendChild(text);
                            fnCell.appendChild(element);
                            fnCell = row.insertCell(1);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].rectAttr[j][k].name);
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(2);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].rectAttr[j][k].value);
                            element.appendChild(text);
                            fnCell.appendChild(element);
                        }
                    window.scrollTo({
                        left: 0,
                        top: 1850,
                        behavior: "smooth"
                    });
                    return;
                    } else {
                        count++;
                    }
                }
                for(let j = 0; j < data[i].numcircles; j++) {
                    if(count == rowId) {
                        row = t4.insertRow(1);
                        fnCell = row.insertCell(0);
                        element = document.createElement('p');
                        text = document.createTextNode("Circle " + (j + 1));
                        element.appendChild(text);
                        fnCell.appendChild(element);
                        for(let k = 0; k < circs[j].numAttr; k++) {
                            row = t5.insertRow(k + 1);
                            fnCell = row.insertCell(0);
                            element = document.createElement('p');
                            text = document.createTextNode((k + 1));
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(1);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].circAttr[j][k].name);
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(2);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].circAttr[j][k].value);
                            element.appendChild(text);
                            fnCell.appendChild(element);
                        }
                    window.scrollTo({
                        left: 0,
                        top: 1850,
                        behavior: "smooth"
                    });
                    return;
                    } else {
                        count++;
                    }
                }
                for(let j = 0; j < data[i].numpaths; j++) {
                    if(count == rowId) {
                        row = t4.insertRow(1);
                        fnCell = row.insertCell(0);
                        element = document.createElement('p');
                        text = document.createTextNode("Path " + (j + 1));
                        element.appendChild(text);
                        fnCell.appendChild(element);
                        for(let k = 0; k < paths[j].numAttr; k++) {
                            row = t5.insertRow(k + 1);
                            fnCell = row.insertCell(0);
                            element = document.createElement('p');
                            text = document.createTextNode((k + 1));
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(1);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].pathAttr[j][k].name);
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(2);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].pathAttr[j][k].value);
                            element.appendChild(text);
                            fnCell.appendChild(element);
                        }
                    window.scrollTo({
                        left: 0,
                        top: 1850,
                        behavior: "smooth"
                    });
                    return;
                    } else {
                        count++;
                    }
                }
                for(let j = 0; j < data[i].numgroups; j++) {
                    if(count == rowId) {
                        row = t4.insertRow(1);
                        fnCell = row.insertCell(0);
                        element = document.createElement('p');
                        text = document.createTextNode("Group " + (j + 1));
                        element.appendChild(text);
                        fnCell.appendChild(element);
                        for(let k = 0; k < groups[j].numAttr; k++) {
                            row = t5.insertRow(k + 1);
                            fnCell = row.insertCell(0);
                            element = document.createElement('p');
                            text = document.createTextNode((k + 1));
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(1);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].groupAttr[j][k].name);
                            element.appendChild(text);
                            fnCell.appendChild(element);

                            fnCell = row.insertCell(2);
                            element = document.createElement('p');
                            text = document.createTextNode(data[i].groupAttr[j][k].value);
                            element.appendChild(text);
                            fnCell.appendChild(element);
                        }
                    window.scrollTo({
                        left: 0,
                        top: 1850,
                        behavior: "smooth"
                    });
                    return;
                    } else {
                        count++;
                    }
                }
            }
        }
    }

    $(function() {  //load animation
        $('body').removeClass('fade-out');
    });

    /*Add a new file*/
    function addImage(data) {   
        document.getElementById("imageForm").style.display = "block";
        var submitBtn = document.getElementById("addFile");
        var closeBtn = document.getElementById("closeFile");
        closeBtn.onclick = function(){
            document.getElementById("imageForm").style.display = "none";
        };

        submitBtn.onclick = function(){
            let filename = document.getElementById("fileName").value;
            let title = document.getElementById("svgTitle").value;
            let description = document.getElementById("svgDescription").value;

            $.ajax({
                type: 'POST',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/sendfile',   //The server endpoint we are connecting to
                data: {filename: filename, title: title, description: description},
            });
            document.getElementById("imageForm").style.display = "none"; 
            window.location.reload();
        };   
    }

    /*Add a new rectangle*/
    function addRect(data,selected) {
        document.getElementById("rectForm").style.display = "block";
        var submitBtn = document.getElementById("subRect");
        var closeBtn = document.getElementById("closeRect");
        closeBtn.onclick = function(){
            document.getElementById("rectForm").style.display = "none";
        };

        submitBtn.onclick = function(){
            let filename = selected;
            let x = document.getElementById("rectX").value;
            let y = document.getElementById("rectY").value;
            let width = document.getElementById("rectW").value;
            let height = document.getElementById("rectH").value;
            let units = document.getElementById("rectU").value;
            $.ajax({
                type: 'POST',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/sendrect',   //The server endpoint we are connecting to
                data: {filename: filename, x: x, y: y, w: width, h: height, units: units},
            });
            document.getElementById("rectForm").style.display = "none";
            window.location.reload();
        };    
    }

    /*Add a new circle*/
    function addCircle(selected) {
        document.getElementById("circleForm").style.display = "block";
        var submitBtn = document.getElementById("subCircle");
        var closeBtn = document.getElementById("closeCircle");
        closeBtn.onclick = function(){
            document.getElementById("circleForm").style.display = "none";
        };

        submitBtn.onclick = function(){
            let filename = selected;
            let cx = document.getElementById("circleCX").value;
            let cy = document.getElementById("circleCY").value;
            let radius = document.getElementById("circleR").value;
            let units = document.getElementById("circleU").value;
            $.ajax({
                type: 'POST',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/sendcircle',   //The server endpoint we are connecting to
                data: { filename: filename, cx: cx, cy: cy, r: radius, units: units },
            });
            document.getElementById("circleForm").style.display = "none";
            window.location.reload();
        };    
    }

    /*Add a new attribute*/
    function addAttr(data,rowId,selected) {
        document.getElementById("attrForm").style.display = "block";
        var submitBtn = document.getElementById("add");
        var closeBtn = document.getElementById("close");
        closeBtn.onclick = function(){
            document.getElementById("attrForm").style.display = "none";
        };
        submitBtn.onclick = function(){
            let filename = selected;
            let name = document.getElementById("name").value;
            let value = document.getElementById("val").value;
           
            $.ajax({
                type: 'POST',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/sendattr',   //The server endpoint we are connecting to
                data: {filename: filename, row: rowId, name: name, value: value},
            });
            document.getElementById("attrForm").style.display = "none";
            window.location.reload();
        };   
    }
});




