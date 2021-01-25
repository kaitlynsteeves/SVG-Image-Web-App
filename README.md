SVG Image Web Application by Kaitlyn Steeves

This application parses uploaded SVG images. It allows the user to view information about the file and information about the elements within the file. 
The application also allows the user to create new SVG images, as well as edit existing ones.

Terminal instructions:
$ cd parser
$ make
$ cd -
$ npm install
$ npm run dev PORT

Website instructions:
CONSOLE LOG PANEL
Uploading a file:
	Press 'select file', select your file, and then press upload. The site will refresh and your new svg file will be there.
Creating a new file:
	Press 'create new' and fill in the fields on the popup. Then press submit.

SVG VIEW PANEL
Selecting an image:
	Use dropdown menu to select.
View a component's other attributes:
	Click on the component which you wish to view. The component will then be selected in light yellow and the view will be shifted down to the attribute view panel.
Add a rectangle/circle:
	Press 'add ____' and fill in the fields on the popup. Then press submit.

ATTRIBUTE VIEW PANEL
Add attributes:
	Press 'Add/Edit' and fill in the fields on the popup. 
Edit attributes:
	Press 'Add/Edit' and fill in the fields on the popup. Make sure the name field matches the name of the attribute you wish to edit, and it will be overwritten.
	* This is also used to exit x, y, width, height, cx, cy, radius, etc. values.



