#include"SVGParser.h"
#include"HelperFunctions.h"
#include<string.h>
#include<math.h>


//A2
/* Public API - main */

/** Function to create an SVG object based on the contents of an SVG file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createSVGimage(char* fileName){
    SVGimage* image = malloc(sizeof(SVGimage));
    xmlNode* head = NULL;
    xmlDoc* doc = NULL;
    
    image->description[0] = '\0';
    image->title[0] = '\0';

    if(checkFileName(fileName) == -1) { 
        free(fileName);
        return NULL;
    }
    
    doc = xmlReadFile(fileName,NULL,0);
    if(doc == NULL) {
        xmlFreeDoc(doc);
         xmlCleanupParser();
        return NULL;
    }

    head = xmlDocGetRootElement(doc);
    if(head == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    /*Gets the namespace*/
    getNameSpace(head,image);
    if(strcmp(image->namespace,"") == 0) {
        deleteSVGimage(image);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    /*Initialize all lists*/
    image->rectangles = initializeList(rectangleToString,deleteRectangle,compareRectangles);
    image->circles = initializeList(circleToString,deleteCircle,compareCircles);
    image->paths = initializeList(pathToString,deletePath,comparePaths);
    image->groups = initializeList(groupToString,deleteGroup,compareGroups);
    image->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
    
    /*Get all of the elements - fill lists*/
    getElements(head, image);
    
    /*Clean up file*/
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return image;
}

/** Function to create a string representation of an SVG object.
 *@pre SVGimgage exists, is not null, and is valid
 *@post SVGimgage has not been modified in any way, and a string representing the SVG contents has been created
 *@return a string contaning a humanly readable representation of an SVG object
 *@param obj - a pointer to an SVG struct
**/
char* SVGimageToString(SVGimage* img){
    char* description;
    char* temp;
    int size = 0;

    if(img == NULL) {
        return NULL;
    }

    /*Find length of everything before malloc*/
    size += strlen("Namespace: ");
    size += strlen(img->namespace);

    if(strcmp(img->title,"") != 0)  { 
        size += strlen("\nTitle: ");
        size += strlen(img->title);
    }

    if(strcmp(img->description,"") != 0) { 
        size += strlen("\nDescription: ");
        size += strlen(img->description);
    }

    if(getLength(img->rectangles) > 0) {
        temp = toString(img->rectangles);
        size += strlen(temp);
        free(temp);
    }
    if(getLength(img->circles) > 0) {
        temp = toString(img->circles);
        size += strlen(temp);
        free(temp);
    }
    if(getLength(img->paths) > 0) {
        temp = toString(img->paths);
        size += strlen(temp);
        free(temp);
    }
    if(getLength(img->groups) > 0) {
        temp = toString(img->groups);
        size += strlen(temp);
        free(temp);
    }

    if(getLength(img->otherAttributes) > 0){
        size += strlen("\n\nSVG Other: ");
        temp = toString(img->otherAttributes);
        size += strlen(temp);
        free(temp);
    }

    description = malloc(sizeof(char)*(size + 1)); 

    /*Copy everything in*/
    strcpy(description,"Namespace: ");
    strcat(description,(img->namespace));

    if(strcmp(img->title,"") != 0)  {
        strcat(description,"\nTitle: ");
        strcat(description,(img->title));
    } 

    if(strcmp(img->description,"") != 0) {
        strcat(description,"\nDescription: ");
        strcat(description,(img->description));
    } 
    
    if(getLength(img->rectangles) > 0) {
        temp = toString(img->rectangles);
        strcat(description,temp);
        free(temp);
    }
    if(getLength(img->circles) > 0) {
       temp = toString(img->circles);
        strcat(description,temp);
        free(temp); 
    }
    if(getLength(img->paths) > 0) {
        temp = toString(img->paths);
        strcat(description,temp);
        free(temp);
    }
    if(getLength(img->groups) > 0) {
        temp = toString(img->groups);
        strcat(description,temp);
        free(temp);
    }

    if(getLength(img->otherAttributes) > 0){
        strcat(description,"\n\nSVG other: ");
        temp = toString(img->otherAttributes);
        strcat(description,temp);
        free(temp);
    }
    description[size] = '\0';
    return description;
}

/** Function to delete image content and free all the memory.
 *@pre SVGimgage  exists, is not null, and has not been freed
 *@post SVSVGimgageG  had been freed
 *@return none
 *@param obj - a pointer to an SVG struct
**/
void deleteSVGimage(SVGimage* img){  
    if(img == NULL) {
        return;
    }

    freeList(img->rectangles);
    freeList(img->paths);
    freeList(img->otherAttributes);
    freeList(img->circles);
    freeList(img->groups);
    free(img);

    return;
}

// Function that returns a list of all rectangles in the image.  
List* getRects(SVGimage* img){
    if(img == NULL) {
        return NULL;
    }

    List *list = initializeList(rectangleToString,deleteRectangle,compareRectangles);
    List *groupList;
    Rectangle *rectangle;
    ListIterator rects = createIterator(img->rectangles);
    ListIterator groups;

    /*Get all regular rectangles*/
    for(int i = 0; i < getLength(img->rectangles); i++) {
        rectangle = (Rectangle*)nextElement(&rects);
        insertBack(list,rectangle);
    }

    /*Get all group rectangles*/
    groupList = getGroups(img);
    groups = createIterator(groupList);
    for(int i = 0; i < getLength(groupList); i++) {
        Group *group = (Group*)nextElement(&groups);
        ListIterator groupRects = createIterator(group->rectangles);
        for(int i = 0; i < getLength(group->rectangles); i++) {
            rectangle = (Rectangle*)nextElement(&groupRects);
            insertBack(list,rectangle);
        }
    }
    safeFreeList(groupList);
    
    return list;
}

// Function that returns a list of all circles in the image.  
List* getCircles(SVGimage* img){
    if(img == NULL) {
        return NULL;
    }

    List *list = initializeList(circleToString,deleteCircle,compareCircles);
    List *groupList;
    Circle *circle;
    ListIterator circs = createIterator(img->circles);
    ListIterator groups;

    /*Get all regular circles*/
    for(int i = 0; i < getLength(img->circles); i++) {
        circle = (Circle*)nextElement(&circs);
        insertBack(list,circle);
    }

    /*Get all group circles*/
    groupList = getGroups(img);
    groups = createIterator(groupList);
    for(int i = 0; i < getLength(groupList); i++) {
        Group *group = (Group*)nextElement(&groups);
        ListIterator groupCircs = createIterator(group->circles);
        for(int i = 0; i < getLength(group->circles); i++) {
            circle = (Circle*)nextElement(&groupCircs);
            insertBack(list,circle);
        }
    }
    safeFreeList(groupList);
    
    return list;
}

// Function that returns a list of all groups in the image.  
List* getGroups(SVGimage* img){
    if(img == NULL) {
        return NULL;
    }

    List *list = initializeList(groupToString,deleteGroup,compareGroups);
    Group *group;
    ListIterator groups = createIterator(img->groups);

    /*Get all regular groups then subgroups with subgroup function*/
    for(int i = 0; i < getLength(img->groups); i++) {
        group = (Group*)nextElement(&groups);
        insertBack(list,group);
        getSubgroup(group,list); 
    }
    
    return list;
}

// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img){
    if(img == NULL) {
        return NULL;
    }

    List *list = initializeList(pathToString,deletePath,comparePaths);
    List *groupList;
    Path *path;
    ListIterator paths = createIterator(img->paths);
    ListIterator groups;

    /*Get all regular paths*/
    for(int i = 0; i < getLength(img->paths); i++) {
        path = (Path*)nextElement(&paths);
        insertBack(list,path);
    }

    /*Get all group paths*/
    groupList = getGroups(img);
    groups = createIterator(groupList);
    for(int i = 0; i < getLength(groupList); i++) {
        Group *group = (Group*)nextElement(&groups);
        ListIterator groupPaths = createIterator(group->paths);
        for(int i = 0; i < getLength(group->paths); i++) {
            path = (Path*)nextElement(&groupPaths);
            insertBack(list,path);
        }
    }
    safeFreeList(groupList);
    
    return list;
}

// Function that returns the number of all rectangles with the specified area

int numRectsWithArea(SVGimage* img, float area){
    if(img == NULL) {
        return -1;
    }

    int num = 0;
    List *rectangles = getRects(img);
    ListIterator iter = createIterator(rectangles);
    
    for(int i = 0; i < getLength(rectangles); i++) {
        Rectangle* rect = (Rectangle*)nextElement(&iter);
        if(getRectArea(rect) == ceil(area)) {
            num++;
        }
    }
    
    return num;
}

// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area){
    if(img == NULL) {
        return -1;
    }

    List *circles = getCircles(img);
    ListIterator iter = createIterator(circles);
    int num = 0;
    
    for(int i = 0; i < getLength(circles); i++) {
        Circle* circle = (Circle*)nextElement(&iter);
        if(getCircleArea(circle) == ceil(area)) {
            num++;
        }
    }
    
    return num;
}

// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data){
    if(img == NULL) {
        return -1;
    }

    List *paths = getPaths(img);
    ListIterator iter = createIterator(paths);
    int num = 0;
    
    for(int i = 0; i < getLength(paths); i++) {
        Path* path = (Path*)nextElement(&iter);
        if(strcmp(path->data,data) == 0) {
            num++;
        }
    }
    
    return num;
}

// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len){
    if(img == NULL) {
        return -1;
    }

    List *groups = getGroups(img);
    ListIterator iter = createIterator(groups);
    int num = 0;

    for(int i = 0; i < getLength(groups); i++) {
        Group *group = (Group*)nextElement(&iter);
        if(getGroupLength(group) == len) {
            num++;
        }
    }

    return num;
}

/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.  
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img){
    int num = 0;
    
    if(img == NULL) {
        return -1;
    }

    num += getLength(img->otherAttributes);
    num += numAttributes(getCircles(img),"circle");
    num += numAttributes(getRects(img),"rectangle");
    num += numAttributes(getPaths(img),"path");
    num += numAttributes(getGroups(img),"group");

    return num;
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteAttribute( void* data){
    Attribute *attr = data;

    free(attr->name);
    free(attr->value);
    free(attr);
}

char* attributeToString( void* data ){
    int size = 0;
    char *string;
    Attribute *attr = data;
    if(attr == NULL) {
        return NULL;
    }
    
    /*Get size before malloc*/
    size += strlen(attr->name);
    size += strlen(attr->value);
    size += strlen("\t: ");

    string = malloc(sizeof(char)*(size + 1)); 

    /*Copy all info in*/
    strcpy(string,"\t");
    strcat(string,attr->name);
    strcat(string,": ");
    strcat(string,attr->value);
    string[size] = '\0';
    return string;
}

int compareAttributes(const void *first, const void *second){
    /*Attribute *struct1 = *(Attribute **)first;
    Attribute *struct2 = *(Attribute **)second;

    if((strcmp(struct1->name,"")!=0) && (strcmp(struct2->name,"")!=0))
    {
        if(strcmp(struct1->name,struct2->name) < 0)
            return (-1);

        else if(strcmp(struct1->name,struct2->name) > 0)
            return (1);

        else
            return 0;
    }
    return -1;*/
    return 0;
}

void deleteGroup(void* data){
    Group *group = data;

    freeList(group->rectangles);
    freeList(group->circles);
    freeList(group->paths);
    freeList(group->groups);
    freeList(group->otherAttributes);
   
    free(group);
}

char* groupToString( void* data){
    int size = 0;
    char *string;
    char *temp;
    Group *group = data;
    if(group == NULL) {
        return NULL;
    }

    /*Get size before malloc*/
    size += strlen("\nGroup: ");
    temp = toString(group->rectangles);
    size += strlen(temp);
    free(temp);
    temp = toString(group->circles);
    size += strlen(temp);
    free(temp);
    temp = toString(group->paths);
    size += strlen(temp);
    free(temp);
    temp = toString(group->groups);
    size += strlen(temp);
    free(temp);

    if(getLength(group->otherAttributes) > 0){
        size += strlen("\nOther: ");
        temp = toString(group->otherAttributes);
        size += strlen(temp);
        free(temp);
    }

    string = malloc(sizeof(char)*size + 1); 

    /*Copy it all in*/
    strcpy(string,"\nGroup: ");
    temp = toString(group->rectangles);
    strcat(string,temp);
    free(temp);
    temp = toString(group->circles);
    strcat(string,temp);
    free(temp);
    temp = toString(group->paths);
    strcat(string,temp);
    free(temp);
    temp = toString(group->groups);
    strcat(string,temp);
    free(temp);
    if(getLength(group->otherAttributes) > 0) {
        strcat(string,"\nOther: ");
        temp = toString(group->otherAttributes);
        strcat(string,temp);
        free(temp);
    }
    string[size] = '\0';
    return string;
}

int compareGroups(const void *first, const void *second){
    return 0;
}

void deleteRectangle(void* data){
    Rectangle *rectangle = data;

    rectangle->height = 0;
    rectangle->width = 0;
    rectangle->x = 0;
    rectangle->y = 0;
    
    freeList(rectangle->otherAttributes);
   
    free(rectangle);
}

char* rectangleToString(void* data){
    int size = 0;                                   
    char *string;
    char *temp;
    Rectangle *rectangle = data;
    if(rectangle == NULL) {
        return NULL;
    }

    /*Get size before malloc*/
    size += strlen("Rectangle: X: %.2f, Y: %.2f, Width: %.2f, Height: %.2f");
    size += sizeof(float) * 4;
    
    if(strcmp(rectangle->units,"") != 0) {
        size += strlen(", Units: "); 
        size += strlen(rectangle->units);
    }

    if(getLength(rectangle->otherAttributes) > 0){
        size += strlen("\n\tOther: ");
        temp = toString(rectangle->otherAttributes);
        size += strlen(temp);
        free(temp);
    }
    
    string = malloc(sizeof(char)*size + 1); 

    /*Copy it all in*/
    sprintf(string,"Rectangle: X: %.2f, Y: %.2f, Width: %.2f, Height: %.2f",rectangle->x,rectangle->y,rectangle->width,rectangle->height);
    if(strcmp(rectangle->units,"") != 0) {
        strcat(string,", Units: ");
        strcat(string,rectangle->units); 
    }
    if(getLength(rectangle->otherAttributes) > 0) {
        strcat(string,"\n\tOther: ");
        temp = toString(rectangle->otherAttributes);
        strcat(string,temp);
        free(temp);
    }
    string[size] = '\0';
    return string;
}

int compareRectangles(const void *first, const void *second){
    return 0;
}

void deleteCircle(void* data){
    Circle *circle = data;

    circle->cx = 0;
    circle->cy = 0;
    circle->r = 0;
    
    //free(circle->units);
    freeList(circle->otherAttributes);
   
    free(circle);
}

char* circleToString(void* data){
    int size = 0;                                     
    char *string;
    Circle *circle = data;
    if(circle == NULL) {
        return NULL;
    }

    /*Get size before malloc*/
    size += strlen("Circle: CX: %.2f, CY: %.2f, Radius: %.2f");
    size += sizeof(float)*3;
    if(strcmp(circle->units,"") != 0) {
        size += strlen(", Units: "); 
        size += strlen(circle->units);
    }
    
    if(getLength(circle->otherAttributes) > 0){
        size += strlen("\n\tOther: ");
        size += strlen(toString(circle->otherAttributes));
    }

    string = malloc(sizeof(char)*size + 1); 

    /*Copy it all in*/
    sprintf(string,"Circle: CX: %.2f, CY: %.2f, Radius: %.2f",circle->cx,circle->cy,circle->r);
    if(strcmp(circle->units,"") != 0) {
        strcat(string,", Units: ");
        strcat(string,circle->units); 
    }
    if(getLength(circle->otherAttributes) > 0) {
        strcat(string,"\n\tOther: ");
        strcat(string,toString(circle->otherAttributes));
    }
    string[size] = '\0';
    return string;
}

int compareCircles(const void *first, const void *second){
    return 0;
}

void deletePath(void* data){
    Path *path = data;

    free(path->data);
    freeList(path->otherAttributes);

    free(path);
}

char* pathToString(void* data){
    int size = 0;                                     
    char *string;
    char *temp;
    Path *path = data;
    if(path == NULL) {
        return NULL;
    }

    /*Get size before malloc*/
    size += strlen("Path: ");
    size += strlen(path->data);

    if(getLength(path->otherAttributes) > 0){
        size += strlen("\n\tOther: ");
        temp = toString(path->otherAttributes);
        size += strlen(temp);
        free(temp);
    }

    string = malloc(sizeof(char)*size + 1); 

    /*Copy it all in*/
    strcpy(string,"Path: ");
    strcat(string,path->data);
    if(getLength(path->otherAttributes) > 0) {
        strcat(string,"\n\tOther: ");
        temp = toString(path->otherAttributes);
        strcat(string,temp);
        free(temp);
    }
    string[size] = '\0';
    return string;
}

int comparePaths(const void *first, const void *second){
    return 0;
}

/************MY HELPER FUNCTIONS**********/

int checkFileName( char* fileName ){
    /*Make sure filename exists and is not null*/
    if(fileName == NULL)
        return -1;

    else if(strcmp(fileName,"") == 0) 
        return -1;
    
    if(strstr(fileName, ".svg") != NULL) 
        return 0;

    return -1;
}

void getNameSpace( xmlNode* head, SVGimage *img ) {
    xmlNode *ptr = NULL;

    for(ptr = head; ptr != NULL; ptr = ptr->next) {
        if (ptr->type == XML_ELEMENT_NODE) {
            if(ptr->ns->href != NULL) {
                strncpy(img->namespace,(char *)(ptr->ns->href),sizeof(char)*256);
                img->namespace[strlen((char *)(ptr->ns->href))] = '\0'; 
            } else {
                /*If there is no namespace, cause error*/
                strcpy(img->namespace,"");
            }
        }
    }
}

/************Function to get all elements, then calls attribute functions within************/
void getElements( xmlNode* head, SVGimage * image ) {
    xmlNode *ptr = NULL;

    for(ptr = head; ptr != NULL; ptr = ptr->next) {
        if(strcmp(((char*)ptr->name), "g") == 0) {

            /*Make group and initialize all lists*/
            Group *group = malloc(sizeof(Group));
            group->paths = initializeList(pathToString,deletePath,comparePaths);
            group->circles = initializeList(circleToString,deleteCircle,compareCircles);
            group->rectangles = initializeList(rectangleToString,deleteRectangle,compareRectangles);
            group->groups = initializeList(groupToString,deleteGroup,compareGroups);
            group->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);

            /*Get other attributes*/
            if(ptr->properties != NULL) {
                xmlAttr *attr = ptr->properties;
                xmlNode *value = attr->children;

                Attribute *attribute = malloc(sizeof(Attribute));

                attribute->name = malloc(sizeof(char)*strlen((char*)attr->name)+1);
                strcpy(attribute->name,(char*)attr->name);
                attribute->name[strlen((char*)attr->name)] = '\0';

                attribute->value = malloc(sizeof(char)*strlen((char*)value->content)+1);
                strcpy(attribute->value,(char*)value->content);
                attribute->value[strlen((char*)value->content)] = '\0';
                insertBack(group->otherAttributes,attribute);
            }

            /*Get regular attributes*/
            getGroupAttributes(ptr->children, group);
            insertBack(image->groups,group);  
            ptr = ptr->next;    /*Increment pointer because this node was taken care of in getGroupAttributes*/
            continue;

        } else if(strcmp(((char*)ptr->name), "path") == 0) {

            /*Create path and get attributes*/
            Path *path = malloc(sizeof(Path));
            path->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getPathAttributes(ptr, path);
            insertBack(image->paths,path);

        } else if(strcmp(((char*)ptr->name), "circle") == 0) {

            /*Create circle and get attributes*/
            Circle *circle = malloc(sizeof(Circle));
            circle->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getCircleAttributes(ptr,circle);
            insertBack(image->circles,circle);

        } else if(strcmp(((char*)ptr->name), "rect") == 0) {

            /*Create rectangle and get attributes*/
            Rectangle *rectangle = malloc(sizeof(Rectangle));
            rectangle->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getRectangleAttributes(ptr, rectangle);
            insertBack(image->rectangles,rectangle);

        } else if(strcmp(((char*)ptr->name), "title") == 0) {

            /*Copy in title if it exists*/
            strncpy(image->title,((char*)ptr->children->content),sizeof(char)*255);
            image->title[255] = '\0';

        } else if(strcmp(((char*)ptr->name), "desc") == 0) {

            /*Copy in description if it exists*/
            strncpy(image->description,((char*)ptr->children->content),sizeof(char)*255);
            image->description[255] = '\0';

        }else {
    
            /*Get other attributes*/
            getOtherAttributes(ptr, image->otherAttributes);

        }
        
        /*Recurse*/
        getElements(ptr->children, image);
    }
    return;
}

/************Functions to get all of the attributes************/

void getRectangleAttributes(xmlNode *ptr, Rectangle *rectangle) {
    xmlAttr *attr;
    strcpy(rectangle->units,"");
        
    for (attr = ptr->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(value != NULL) {
            if(strcmp(rectangle->units,"") == 0) {
                strncpy(rectangle->units, getUnits((char*)attr->children->content),sizeof(char)*50); /*Get units*/
            }
            if(strcmp((char*)attr->name,"x") == 0) {
                rectangle->x = atof((char*)value->content);
            } else if(strcmp((char*)attr->name,"y") == 0) {
                rectangle->y = atof((char*)value->content);
            } else if(strcmp((char*)attr->name,"width") == 0) {
                rectangle->width = atof((char*)value->content);
            } else if(strcmp((char*)attr->name,"height") == 0) {
                rectangle->height = atof((char*)value->content);
            } else {
                Attribute *attribute = malloc(sizeof(Attribute));

                attribute->name = malloc(sizeof(char)*strlen((char*)attr->name)+1);
                strcpy(attribute->name,(char*)attr->name);
                attribute->name[strlen((char*)attr->name)] = '\0';

                attribute->value = malloc(sizeof(char)*strlen((char*)value->content)+1);
                strcpy(attribute->value,(char*)value->content);
                attribute->value[strlen((char*)value->content)] = '\0';
                insertBack(rectangle->otherAttributes,attribute);
            }
        }
    }
}

void getCircleAttributes(xmlNode *ptr, Circle *circle) {
    xmlAttr *attr;
    strcpy(circle->units,"");
        
    for (attr = ptr->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(value != NULL) {
            if(strcmp(circle->units,"") == 0) {
                strncpy(circle->units, getUnits((char*)attr->children->content),50);
            }
            if(strcmp((char*)attr->name,"cx") == 0) {
                circle->cx = atof((char*)value->content);
            } else if(strcmp((char*)attr->name,"cy") == 0) {
                circle->cy = atof((char*)value->content);
            } else if(strcmp((char*)attr->name,"r") == 0) {
                circle->r = atof((char*)value->content);    
            } else {
                Attribute *attribute = malloc(sizeof(Attribute));

                attribute->name = malloc(sizeof(char)*strlen((char*)attr->name)+1);
                strcpy(attribute->name,(char*)attr->name);
                attribute->name[strlen((char*)attr->name)] = '\0';

                attribute->value = malloc(sizeof(char)*strlen((char*)value->content)+1);
                strcpy(attribute->value,(char*)value->content);
                attribute->value[strlen((char*)value->content)] = '\0';
                insertBack(circle->otherAttributes,attribute);
            }
        }
    }
}

void getPathAttributes(xmlNode *ptr, Path *path){
    xmlAttr *attr;
        
    for (attr = ptr->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(value != NULL) {
            if(strcmp((char*)attr->name,"d") == 0) {
                path->data = malloc(sizeof(char)*strlen((char*)value->content)+1);
                strcpy(path->data,(char*)value->content);
                path->data[strlen((char*)value->content)] = '\0'; 
            } else {
                Attribute *attribute = malloc(sizeof(Attribute));

                attribute->name = malloc(sizeof(char)*strlen((char*)attr->name)+1);
                strcpy(attribute->name,(char*)attr->name);
                attribute->name[strlen((char*)attr->name)] = '\0';

                attribute->value = malloc(sizeof(char)*strlen((char*)value->content)+1);
                strcpy(attribute->value,(char*)value->content);
                attribute->value[strlen((char*)value->content)] = '\0';
                insertBack(path->otherAttributes,attribute);
            }
        }
    }
}

void getGroupAttributes(xmlNode *head, Group *group) {
    xmlNode *ptr;    

    /*Fill lists similar to getElements*/
    for(ptr = head; ptr != NULL; ptr = ptr->next) {
        if(strcmp(((char*)ptr->name), "g") == 0) {

            Group *subgroup = malloc(sizeof(Group));
            subgroup->paths = initializeList(pathToString,deletePath,comparePaths);
            subgroup->circles = initializeList(circleToString,deleteCircle,compareCircles);
            subgroup->rectangles = initializeList(rectangleToString,deleteRectangle,compareRectangles);
            subgroup->groups = initializeList(groupToString,deleteGroup,compareGroups);
            subgroup->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getGroupAttributes(ptr->children, subgroup);
            insertBack(group->groups,subgroup);  

        } else if(strcmp(((char*)ptr->name), "path") == 0) {

            Path *path = malloc(sizeof(Path));
            path->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getPathAttributes(ptr, path);
            insertBack(group->paths,path);

        } else if(strcmp(((char*)ptr->name), "circle") == 0) {

            Circle *circle = malloc(sizeof(Circle));
            circle->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getCircleAttributes(ptr,circle);
            insertBack(group->circles,circle);

        } else if(strcmp(((char*)ptr->name), "rect") == 0) {

            Rectangle *rectangle = malloc(sizeof(Rectangle));
            rectangle->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
            getRectangleAttributes(ptr, rectangle);
            insertBack(group->rectangles,rectangle);

        }     
    }
}

void getOtherAttributes(xmlNode *ptr, List *list) {
    xmlAttr *attr;
        
    for (attr = ptr->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(value != NULL) {
            Attribute *attribute = malloc(sizeof(Attribute));

            attribute->name = malloc(sizeof(char)*strlen((char*)attr->name)+1);
            strcpy(attribute->name,(char*)attr->name);
            attribute->name[strlen((char*)attr->name)] = '\0';

            attribute->value = malloc(sizeof(char)*strlen((char*)value->content)+1);
            strcpy(attribute->value,(char*)value->content);
            attribute->value[strlen((char*)value->content)] = '\0';

            insertBack(list,attribute);
        }
    }
}

/*Gets units*/
char* getUnits(char *string) {
    if(strstr(string, "mm") != NULL) {
        return "mm";
    } else if(strstr(string, "cm") != NULL) {
        return "cm";
    } else if(strstr(string, "px") != NULL) {
        return "px";
    }
    return "";
}

/*Gets subgroups*/
void getSubgroup(Group *group, List* list) {
    ListIterator groups = createIterator(group->groups);

    for(int i = 0; i < getLength(group->groups); i++) {
        Group *subgroup = (Group*)nextElement(&groups);
        insertBack(list,subgroup);
        getSubgroup(subgroup,list);
    }
}

float getRectArea(Rectangle *rect) {
    return ceil(rect->height*rect->width);
}

float getCircleArea(Circle *circle) {
    return ceil(3.14159*pow(circle->r,2));
}

int getGroupLength(Group *group) {
    return(getLength(group->circles) + getLength(group->groups) + getLength(group->rectangles) + getLength(group->paths));
}

int numAttributes(List *list, char* type) {
    int num = 0;
    ListIterator iter = createIterator(list);
    
    /*Use the type to determine which length to get*/
    for(int i = 0; i < getLength(list); i++) {
        if(strcmp(type,"rectangle") == 0) {
            Rectangle* rect = (Rectangle*)nextElement(&iter);
            num += getLength(rect->otherAttributes);
        } else if(strcmp(type,"circle") == 0) {
            Circle* circ = (Circle*)nextElement(&iter);
            num += getLength(circ->otherAttributes);
        } else if(strcmp(type,"path") == 0) {
            Path* path = (Path*)nextElement(&iter);
            num += getLength(path->otherAttributes);
        } else if(strcmp(type,"group") == 0) {
            Group* group = (Group*)nextElement(&iter);
            num += getLength(group->otherAttributes);
        }
    }
    return num;
}

/* ************************************************ A2 stuff ************************************************** */
/** Function to validate an existing a SVGimage object against a SVG schema file
 *@pre 
    SVGimage object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post SVGimage has not been modified in any way
 *@return the boolean aud indicating whether the SVGimage is valid
 *@param obj - a pointer to a GPXSVGimagedoc struct
 *@param obj - the name iof a schema file
 **/
bool validateSVGimage(SVGimage* image, char* schemaFile){
    xmlDoc *doc = NULL;
    List *list = NULL;
    ListIterator iter;

    // check restraints 
    if((image == NULL) || (checkSchemaName(schemaFile) == -1) || (image->namespace[0] == '\0')) {
        return false;
    }
    
    if((image->rectangles == NULL) || (image->circles == NULL) || (image->paths == NULL) || (image->groups == NULL) || (image->otherAttributes == NULL)) {
        return NULL;
    }

    //validate 
    if(validateAttributes(image->otherAttributes) != 0)
    {
        return false;
    }

    //rectangles
    list = getRects(image);
    iter = createIterator(list);
    for(int i = 0; i < getLength(list); i++) {
        Rectangle *rect = (Rectangle*)nextElement(&iter);
        if((rect->width < 0) || (rect->height < 0) || (rect->otherAttributes == NULL)) {
            return false;
        }
        if(validateAttributes(rect->otherAttributes) != 0)
        {
            return false;
        }
    }
    safeFreeList(list);
    
    //circles
    list = getCircles(image);
    iter = createIterator(list);
    for(int i = 0; i < getLength(list); i++) {
        Circle *circle = (Circle*)nextElement(&iter);
        if((circle->r < 0) || (circle->otherAttributes == NULL)) {
            return false;
        }
        if(validateAttributes(circle->otherAttributes) != 0)
        {
            return false;
        }
    }
    safeFreeList(list);

    //paths
    list = getPaths(image);
    iter = createIterator(list);
    for(int i = 0; i < getLength(list); i++) {
        Path *path = (Path*)nextElement(&iter);
        if((path->data == NULL) || (path->otherAttributes == NULL)) {
            return false;
        }
        if(validateAttributes(path->otherAttributes) != 0)
        {
            return false;
        }
    }
    safeFreeList(list);
    
    //groups 
    list = getGroups(image);
    iter = createIterator(list);
    for(int i = 0; i < getLength(list); i++) {
        Group *group = (Group*)nextElement(&iter);
        if((group->rectangles == NULL) || (group->circles == NULL) || (group->paths == NULL) || (group->groups == NULL) || (group->otherAttributes == NULL)) {
            return false;
        }
        if(validateAttributes(group->otherAttributes) != 0)
        {
            return false;
        }
    }
    safeFreeList(list);
    
    doc = createXmlDoc(image);

    if(checkDoc(doc,schemaFile) != 0) {
        return false;
    }
    
    return true;
}

/** Function to create an SVG object based on the contents of an SVG file.
 * This function must validate the XML tree generated by libxml against a SVG schema file
 * before attempting to traverse the tree and create an SVGimage struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
       Schema file name is not NULL/empty, and represents a valid schema file
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, or SVG file was invalid, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createValidSVGimage(char* fileName, char* schemaFile){
    xmlDoc* doc = NULL;

    if((checkFileName(fileName) == -1) || (checkSchemaName(schemaFile) == -1)) { 
        free(fileName);
        free(schemaFile);
        return NULL;
    }

    /*Get the doc*/
    doc = xmlReadFile(fileName,NULL,0);
    if(doc == NULL) {
        xmlFreeDoc(doc);
        return NULL;
    }

    /*Validate against schema*/
    if(checkDoc(doc, schemaFile) != 0){
        xmlFreeDoc(doc);
        return NULL;
    }
    
    return createSVGimage(fileName);
}

/** Function to writing a SVGimage into a file in SVG format.
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post SVGimage has not been modified in any way, and a file representing the
    SVGimage contents in SVG format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a SVGimage struct
 	fileName - the name of the output file
 **/
bool writeSVGimage(SVGimage* image, char* fileName){
    xmlDoc *doc;
    int ret;
    
    if((image == NULL) || checkFileName(fileName) != 0) {
        return false;
    }

    doc = createXmlDoc(image);
    ret = xmlSaveFormatFile(fileName, doc, 1);
    xmlFreeDoc(doc);

    if(ret == -1) {
        return false;
    }

    return true;
}

/** Function to setting an attribute in an SVGimage or component
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newAttribute is not NULL
 *@post The appropriate attribute was set corectly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    elemIndex - index of thje lement to modify
    newAttribute - struct containing name and value of the updated attribute
 **/
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
    List *attributes;
    ListIterator iter;
    
    if(image == NULL) {
        return;
    }

    //make sure index is valid
    if( elemType != SVG_IMAGE && elemIndex < 0) {
        return;
    }

    switch(elemType) {
        case SVG_IMAGE: 
            attributes = image->otherAttributes;
            iter = createIterator(attributes);
            break;

        case CIRC: 
            if(elemIndex > getLength(image->circles)) {
                return;
            }
            Circle *circle;
            iter = createIterator(image->circles);
            for(int i = 0; i < elemIndex; i++) {
                circle = (Circle*)nextElement(&iter);
                
            }
            printf("%d\n",getLength(circle->otherAttributes));
            if(strcmp(newAttribute->name,"cx") == 0) {
                circle->cx = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else if(strcmp(newAttribute->name,"cy") == 0) {
                circle->cy = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else if(strcmp(newAttribute->name,"r") == 0) {
                circle->r = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } 
            
            if(getLength(circle->otherAttributes) == 0) {
                insertBack(circle->otherAttributes,newAttribute);
                return;
            }
            attributes = circle->otherAttributes;
            iter = createIterator(attributes);
            break;

        case RECT: 
            if(elemIndex > getLength(image->rectangles)) {
                return;
            }
            Rectangle *rect;
            iter = createIterator(image->rectangles);
            for(int i = 0; i < elemIndex; i++) {
                rect = (Rectangle*)nextElement(&iter);
            }
            
            if(strcmp(newAttribute->name,"x") == 0) {
                rect->x = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else if(strcmp(newAttribute->name,"y") == 0) {
                rect->y = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else if(strcmp(newAttribute->name,"width") == 0) {
                rect->width = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else if(strcmp(newAttribute->name,"height") == 0) {
                rect->height = atof(newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            }

            attributes = rect->otherAttributes;
            iter = createIterator(attributes);
            break;

        case PATH:
            if(elemIndex > getLength(image->paths)) {
                return;
            }
            Path *path;
            iter = createIterator(image->paths);
            for(int i = 0; i <= elemIndex; i++) {
                path = (Path*)nextElement(&iter);
            }

            if(strcmp(newAttribute->name,"d") == 0) {
                strcpy(path->data,newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } 

            attributes = path->otherAttributes;
            iter = createIterator(attributes);
            break;

        case GROUP:
            if(elemIndex > getLength(image->groups)) {
                return;
            }
            Group *group;
            iter = createIterator(image->groups);
            for(int i = 0; i <= elemIndex; i++) {
                group = (Group*)nextElement(&iter);
            }
            attributes = group->otherAttributes;
            iter = createIterator(attributes);
            break;
        
        default:
            deleteAttribute(newAttribute);
    }

    for(int i = 0; i < getLength(attributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&iter);
        if(strcmp(attr->name,newAttribute->name) == 0) {
            attr->value = realloc(attr->value,sizeof(char)*strlen(newAttribute->value) + 1);
            strcpy(attr->value,newAttribute->value);
            attr->value[strlen(newAttribute->value)] = '\0';
            deleteAttribute(newAttribute);
            return;
        }
    }
    
    insertBack(attributes,newAttribute);
    
    return;
}

/** Function to adding an element - Circle, Rectngle, or Path - to an SVGimage
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newElement is not NULL
 *@post The appropriate element was added correctly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    newElement - pointer to the element sgtruct (Circle, Rectngle, or Path)
 **/
void addComponent(SVGimage* image, elementType type, void* newElement) {
    if(image == NULL) {
        return;
    }

    switch(type) {
        case CIRC: 
            insertBack(image->circles,(Circle*)newElement);
            break;

        case RECT: 
            insertBack(image->rectangles,(Rectangle*)newElement);
            break;

        case PATH:
            insertBack(image->paths,(Path*)newElement);
            break;

        default:    
            return;
    }

    return;
}

/******A2 HELPER FUNCTIONS*/

int checkSchemaName( char* fileName ){
    /*Make sure filename exists and is not null*/
    if(fileName == NULL)
        return -1;
        
    else if(strcmp(fileName,"") == 0) 
        return -1;
    
    if(strstr(fileName, ".xsd") != NULL)
        return 0;

    return -1;
}

int checkDoc(xmlDoc *doc, char* schemaFile) {
    int ret;
    xmlSchemaValidCtxtPtr ctxt = NULL;
    xmlSchemaParserCtxtPtr parser = NULL;
    xmlSchemaPtr schema = NULL;
    
    /*Set up schema*/
    parser = xmlSchemaNewParserCtxt(schemaFile);
    schema = xmlSchemaParse(parser);
    xmlSchemaFreeParserCtxt(parser);
    if(schema == NULL) {
        xmlSchemaFree(schema);
        xmlFreeDoc(doc);
        xmlSchemaCleanupTypes();
        xmlMemoryDump();
        return -1;
    }

    ctxt = xmlSchemaNewValidCtxt(schema);
    ret = xmlSchemaValidateDoc(ctxt, doc);

    /*Cleanup*/
    xmlSchemaFreeValidCtxt(ctxt);
    xmlSchemaFree(schema);
    xmlFreeDoc(doc);
    xmlSchemaCleanupTypes();
    xmlMemoryDump();
    
    return ret;
}

xmlDoc* createXmlDoc(SVGimage *img) {
    xmlDoc *doc = xmlNewDoc((xmlChar*)"1.0");
    xmlNode *root = xmlNewNode(NULL,(xmlChar*)"svg");
    xmlNsPtr ns =  xmlNewNs(root, (xmlChar*)img->namespace, NULL);
    ListIterator iter;

    xmlDocSetRootElement(doc,root);
    xmlSetNs(root,ns);
    if (img == NULL)
    {
        return NULL;
    }
    
    if(img->title[0] != '\0') {
        xmlNewChild(root, NULL, (xmlChar*)"title", (xmlChar*)img->title);
    }
    if(img->description[0] != '\0') {
         xmlNewChild(root, NULL, (xmlChar*)"desc", (xmlChar*)img->description);
    }

    iter = createIterator(img->otherAttributes);
    for(int i = 0; i < getLength(img->otherAttributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&iter);
        xmlNewProp(root,(xmlChar*)attr->name,(xmlChar*)attr->value);
    }

    iter = createIterator(img->rectangles);
    for(int i = 0; i < getLength(img->rectangles); i++) {
        Rectangle *rectangle = (Rectangle*)nextElement(&iter);
        addRectToDoc(root,rectangle);
    }

    iter = createIterator(img->circles);
    for(int i = 0; i < getLength(img->circles); i++) {
        Circle *circle = (Circle*)nextElement(&iter);
        addCircleToDoc(root,circle);
    }

    iter = createIterator(img->paths);
    for(int i = 0; i < getLength(img->paths); i++) {
        Path *path = (Path*)nextElement(&iter);
        addPathToDoc(root,path);
    }
    
    iter = createIterator(img->groups);
    for(int i = 0; i < getLength(img->groups); i++) {
        Group *group = (Group*)nextElement(&iter);
        addGroupToDoc(root,group);
    }

    return doc;
}

void addRectToDoc(xmlNode *root, Rectangle *rect) {
    char temp[50];
    ListIterator attributes = createIterator(rect->otherAttributes);
    xmlNode *node = xmlNewChild(root, NULL, (xmlChar*)"rect", NULL);

    sprintf(temp,"%.2f",rect->x);
    if(validateUnits(rect->units) == 0) { 
        strcat(temp,rect->units);
    }
    xmlNewProp(node,(xmlChar*)"x",(xmlChar*)temp);
    
    sprintf(temp,"%.2f",rect->y);
    if(validateUnits(rect->units) == 0) { 
        strcat(temp,rect->units);
    }
    xmlNewProp(node,(xmlChar*)"y",(xmlChar*)temp);

    sprintf(temp,"%.2f",rect->width);
    if(validateUnits(rect->units) == 0) { 
        strcat(temp,rect->units);
    }
    xmlNewProp(node,(xmlChar*)"width",(xmlChar*)temp);

    sprintf(temp,"%.2f",rect->height);
    if(validateUnits(rect->units) == 0) { 
        strcat(temp,rect->units);
    }
    xmlNewProp(node,(xmlChar*)"height",(xmlChar*)temp);

    //units should already be going in?

    for(int i = 0; i < getLength(rect->otherAttributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&attributes);
        xmlNewProp(node,(xmlChar*)attr->name,(xmlChar*)attr->value);
    }

    return;
}

void addCircleToDoc(xmlNode *root, Circle *circle) {
    char temp[50];
    ListIterator attributes = createIterator(circle->otherAttributes);
    xmlNode *node = xmlNewChild(root, NULL, (xmlChar*)"circle", NULL);

    sprintf(temp,"%.2f",circle->cx);
    if(validateUnits(circle->units) == 0) { 
        strcat(temp,circle->units);
    }
    xmlNewProp(node,(xmlChar*)"cx",(xmlChar*)temp);
    
    sprintf(temp,"%.2f",circle->cy);
    if(validateUnits(circle->units) == 0) { 
        strcat(temp,circle->units);
    }
    xmlNewProp(node,(xmlChar*)"cy",(xmlChar*)temp);

    sprintf(temp,"%.2f",circle->r);
    if(validateUnits(circle->units) == 0) { 
        strcat(temp,circle->units);
    }
    xmlNewProp(node,(xmlChar*)"r",(xmlChar*)temp);

    for(int i = 0; i < getLength(circle->otherAttributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&attributes);
        xmlNewProp(node,(xmlChar*)attr->name,(xmlChar*)attr->value);
    }

    return;
}

void addPathToDoc(xmlNode *root, Path *path) {
    ListIterator attributes = createIterator(path->otherAttributes);
    xmlNode *node = xmlNewChild(root, NULL, (xmlChar*)"path", NULL);

    xmlNewProp(node,(xmlChar*)"d",(xmlChar*)path->data);

    for(int i = 0; i < getLength(path->otherAttributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&attributes);
        xmlNewProp(node,(xmlChar*)attr->name,(xmlChar*)attr->value);
    }

    return;
}

void addGroupToDoc(xmlNode *root, Group *group) {
    xmlNode *node = xmlNewChild(root, NULL, (xmlChar*)"g", NULL);
    ListIterator iter;

    iter = createIterator(group->rectangles);
    for(int i = 0; i < getLength(group->rectangles); i++) {
        Rectangle *rectangle = (Rectangle*)nextElement(&iter);
        addRectToDoc(node,rectangle);
    }

    iter = createIterator(group->circles);
    for(int i = 0; i < getLength(group->circles); i++) {
        Circle *circle = (Circle*)nextElement(&iter);
        addCircleToDoc(node,circle);
    }

    iter = createIterator(group->paths);
    for(int i = 0; i < getLength(group->paths); i++) {
        Path *path = (Path*)nextElement(&iter);
        addPathToDoc(node,path);
    }
    
    iter = createIterator(group->groups);
    for(int i = 0; i < getLength(group->groups); i++) {
        Group *group = (Group*)nextElement(&iter);
        addGroupToDoc(node,group);
    }

    iter = createIterator(group->otherAttributes);
    for(int i = 0; i < getLength(group->otherAttributes); i++) {
        Attribute *attr = (Attribute*)nextElement(&iter);
        xmlNewProp(node,(xmlChar*)attr->name,(xmlChar*)attr->value);
    }

    return;
}

void safeFreeList(List *list) {
    Node* tmp;

    if (list == NULL){
		return;
	}
	
	if (list->head == NULL && list->tail == NULL){
        free(list);
		return;
	}
	
	while (list->head != NULL){
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}
	
	list->head = NULL;
	list->tail = NULL;
	list->length = 0;
    free(list);
    return;
}

int validateUnits(char units[50]) {
    if(units == NULL) {
        return -1;
    } else if (units[0] == '\0') {
        return -1;
    } else if(strcmp(units,"") == 0) {
        return -1;
    } 
    return 0;
}

int validateAttributes(List *list) {
    ListIterator iter = createIterator(list);
    
    for(int i = 0; i < getLength(list); i++) {
        Attribute *attr = (Attribute*)nextElement(&iter);
        if(attr->name == NULL || attr->value == NULL) {
            return -1;
        }
    }

    return 0;
}

/** Function to converting an Attribute into a JSON string
*@pre Attribute is not NULL
*@post Attribute has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an Attribute struct
**/
char* attrToJSON(const Attribute *a) {
    char *string;
    int size = 0;
    
    if(a == NULL) {
        return "{}";
    }

    size += strlen("{\"name\":\"");
    size += strlen(a->name);
    size += strlen("\",\"value\":\"");
    size += strlen(a->value);
    size += strlen("\"}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"name\":\"");
    strcat(string,a->name);
    strcat(string,"\",\"value\":\"");
    strcat(string,a->value);
    strcat(string,"\"}");
    string[size] = '\0';

    return string;
}

/** Function to converting a Circle into a JSON string
*@pre Circle is not NULL
*@post Circle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Circle struct
**/
char* circleToJSON(const Circle *c) {
    char *string;
    char temp[256];
    int size = 0;
    
    if(c == NULL) {
        return "{}";
    }

    size += strlen("{\"cx\":");
    sprintf(temp,"%.2f",c->cx);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"cy\":");
    sprintf(temp,"%.2f",c->cy);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"r\":");
    sprintf(temp,"%.2f",c->r);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"numAttr\":");
    sprintf(temp,"%d",getLength(c->otherAttributes));
    size+= strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"units\":\"");
    size += strlen(c->units);
    size += strlen("\"}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"cx\":");
    sprintf(temp,"%.2f",c->cx);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"cy\":");
    sprintf(temp,"%.2f",c->cy);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"r\":");
    sprintf(temp,"%.2f",c->r);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"numAttr\":");
    sprintf(temp,"%d",getLength(c->otherAttributes));
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"units\":\"");
    strcat(string,c->units);
    strcat(string,"\"}");

    return string;
}

/** Function to converting a Rectangle into a JSON string
*@pre Rectangle is not NULL
*@post Rectangle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Rectangle struct
**/
char* rectToJSON(const Rectangle *r) {
    char *string;
    char temp[256];
    int size = 0;
    
    if(r == NULL) {
        return "{}";
    }

    size += strlen("{\"x\":");
    sprintf(temp,"%.2f",r->x);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"y\":");
    sprintf(temp,"%.2f",r->y);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"w\":");
    sprintf(temp,"%.2f",r->width);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"h\":");
    sprintf(temp,"%.2f",r->height);
    size += strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"numAttr\":");
    sprintf(temp,"%d",getLength(r->otherAttributes));
    size+= strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"units\":\"");
    size += strlen(r->units);
    size += strlen("\"}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"x\":");
    sprintf(temp,"%.2f",r->x);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"y\":");
    sprintf(temp,"%.2f",r->y);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"w\":");
    sprintf(temp,"%.2f",r->width);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"h\":");
    sprintf(temp,"%.2f",r->height);
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"numAttr\":");
    sprintf(temp,"%d",getLength(r->otherAttributes));
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"units\":\"");
    strcat(string,r->units);
    strcat(string,"\"}");

    return string;
}

/** Function to converting a Path into a JSON string
*@pre Path is not NULL
*@post Path has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Path struct
**/
char* pathToJSON(const Path *p) {
    char *string;
    char temp[256];
    int size = 0;
    
    if(p == NULL) {
        return "{}";
    }

    size += strlen("{\"d\":\"");
    if(strlen(p->data) < 64) {
        size += strlen(p->data);
    } else {
        size += 64;
    }
    
    size += strlen("\",\"numAttr\":");
    sprintf(temp,"%d",getLength(p->otherAttributes));
    size+= strlen(temp);
    size += strlen("}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"d\":\"");
    if(strlen(p->data) < 64) {
        strcat(string,p->data);
    } else {
        strncat(string,p->data,64);
    }
    strcat(string,"\",\"numAttr\":");
    strcat(string,temp);
    strcat(string,"}");
    string[size] = '\0';

    return string;
}

/** Function to converting a Group into a JSON string
*@pre Group is not NULL
*@post Group has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Group struct
**/
char* groupToJSON(const Group *g) {
    char *string;
    char temp[256];
    int size = 0;
    
    if(g == NULL) {
        return "{}";
    }

    size += strlen("{\"children\":");
    sprintf(temp,"%d",(getLength(g->groups) + getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths)));
    size+= strlen(temp);
    temp[0] = '\0';
    size += strlen(",\"numAttr\":");
    sprintf(temp,"%d",getLength(g->otherAttributes));
    size+= strlen(temp);
    temp[0] = '\0';
    size += strlen("}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"children\":");
    sprintf(temp,"%d",(getLength(g->groups) + getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths)));
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,",\"numAttr\":");
    sprintf(temp,"%d",getLength(g->otherAttributes));
    strcat(string,temp);
    temp[0] = '\0';
    strcat(string,"}");
    string[size] = '\0';

    return string;
}

/** Function to converting a list of Attribute structs into a JSON string
*@pre Attribute list is not NULL
*@post Attribute list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* attrListToJSON(const List *list) {
    if(list == NULL) {
        return "[]";
    }
    
    int size = 0;
    char *string;
    char *desc;
    Attribute *temp;
    ListIterator iter = createIterator((List*)list);

    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            size += 1;
        }
        temp = (Attribute*)nextElement(&iter);
        desc = attrToJSON(temp);
        size += strlen(desc);
        free(desc);
    }

    size += 2;
    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"[");
    iter = createIterator((List*)list);
    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            strcat(string,","); 
        }
        temp = (Attribute*)nextElement(&iter);
        desc = attrToJSON(temp);
        strcat(string,desc);
        free(desc);
    }
    strcat(string,"]");
    string[size] = '\0';

    return string;
}

/** Function to converting a list of Circle structs into a JSON string
*@pre Circle list is not NULL
*@post Circle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* circListToJSON(const List *list) {
    if(list == NULL) {
        return "[]";
    }
    
    int size = 0;
    char *string;
    char *desc;
    Circle *temp;
    ListIterator iter = createIterator((List*)list);

    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            size += 1;
        }
        temp = (Circle*)nextElement(&iter);
        desc = circleToJSON(temp);
        size += strlen(desc);
        free(desc);
    }

    size += 2;
    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"[");
    iter = createIterator((List*)list);
    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            strcat(string,","); 
        }
        temp = (Circle*)nextElement(&iter);
        desc = circleToJSON(temp);
        strcat(string,desc);
        free(desc);
    }
    strcat(string,"]");
    string[size] = '\0';

    return string;
}

/** Function to converting a list of Rectangle structs into a JSON string
*@pre Rectangle list is not NULL
*@post Rectangle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* rectListToJSON(const List *list) {
    if(list == NULL) {
        return "[]";
    }
    
    int size = 0;
    char *string;
    char *desc;
    Rectangle *temp;
    ListIterator iter = createIterator((List*)list);

    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            size += 1;
        }
        temp = (Rectangle*)nextElement(&iter);
        desc = rectToJSON(temp);
        size += strlen(desc);
        free(desc);
    }

    size += 2;
    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"[");
    iter = createIterator((List*)list);
    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            strcat(string,","); 
        }
        temp = (Rectangle*)nextElement(&iter);
        desc = rectToJSON(temp);
        strcat(string,desc);
        free(desc);
    }
    strcat(string,"]");
    string[size] = '\0';

    return string;
}

/** Function to converting a list of Path structs into a JSON string
*@pre Path list is not NULL
*@post Path list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* pathListToJSON(const List *list) {
    if(list == NULL) {
        return "[]";
    }
    
    int size = 0;
    char *string;
    char *desc;
    Path *temp;
    ListIterator iter = createIterator((List*)list);

    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            size += 1;
        }
        temp = (Path*)nextElement(&iter);
        desc = pathToJSON(temp);
        size += strlen(desc);
        free(desc);
    }

    size += 2;
    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"[");
    iter = createIterator((List*)list);
    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            strcat(string,","); 
        }
        temp = (Path*)nextElement(&iter);
        desc = pathToJSON(temp);
        strcat(string,desc);
        free(desc);
    }
    strcat(string,"]");
    string[size] = '\0';

    return string;
}

/** Function to converting a list of Group structs into a JSON string
*@pre Group list is not NULL
*@post Group list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* groupListToJSON(const List *list) {
    if(list == NULL) {
        return "[]";
    }
    
    int size = 0;
    char *string;
    char *desc;
    Group *temp;
    ListIterator iter = createIterator((List*)list);

    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            size += 1;
        }
        temp = (Group*)nextElement(&iter);
        desc = groupToJSON(temp);
        size += strlen(desc);
        free(desc);
    }

    size += 2;
    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"[");
    iter = createIterator((List*)list);
    for(int i = 0; i < getLength((List*)list); i++) {
        if(i > 0) {
            strcat(string,","); 
        }
        temp = (Group*)nextElement(&iter);
        desc = groupToJSON(temp);
        strcat(string,desc);
        free(desc);
    }
    strcat(string,"]");
    string[size] = '\0';

    return string;
}

/** Function to converting an SVGimage into a JSON string
*@pre SVGimage is not NULL
*@post SVGimage has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an SVGimage struct
**/
char* SVGtoJSON(const SVGimage* imge) {
    char *string;
    char temp[256];
    int size = 0;
    List *list;
    
    if(imge == NULL) {
        return "{}";
    } 

    size += strlen("{\"numRect\":");
    list = getRects((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    size+= strlen(temp);
    temp[0] = '\0';
    safeFreeList(list);

    size += strlen(",\"numCirc\":");
    list = getCircles((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    size+= strlen(temp);
    temp[0] = '\0';
    safeFreeList(list);

    size += strlen(",\"numPaths\":");
    list = getPaths((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    size+= strlen(temp);
    temp[0] = '\0';
    safeFreeList(list);

    size += strlen(",\"numGroups\":");
    list = getGroups((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    size+= strlen(temp);
    temp[0] = '\0';
    safeFreeList(list);

    size += strlen("}");

    string = malloc(sizeof(char)*size + 1);

    strcpy(string,"{\"numRect\":");
    list = getRects((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    strcat(string,temp);
    temp[0] = '\0';
    safeFreeList(list);

    strcat(string,",\"numCirc\":");
    list = getCircles((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    strcat(string,temp);
    temp[0] = '\0';
    safeFreeList(list);

    strcat(string,",\"numPaths\":");
    list = getPaths((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    strcat(string,temp);
    temp[0] = '\0';
    safeFreeList(list);

    strcat(string,",\"numGroups\":");
    list = getGroups((SVGimage*)imge);
    sprintf(temp,"%d",getLength(list));
    strcat(string,temp);
    temp[0] = '\0';
    safeFreeList(list);

    strcat(string,"}");
    string[size] = '\0';

    return string;
}

/* ******************************* Bonus A2 functions - optional for A2 *************************** */

/** Function to converting a JSON string into an SVGimage struct
*@pre JSON string is not NULL
*@post String has not been modified in any way
*@return A newly allocated and initialized SVGimage struct
*@param str - a pointer to a string
**/
SVGimage* JSONtoSVG(const char* svgString) {
    if(svgString == NULL) {
        return NULL;
    }
    char *sub[4];
    char *string = malloc(strlen(svgString) + 1);
    SVGimage* image = malloc(sizeof(SVGimage));
    
    strcpy(string,svgString);
    string[strlen(string)] = '\0';
    removeExtras(string);

    sub[0] = strtok(string, ",:");
    removeExtras(sub[0]);
    for(int i = 1; i < 4; i++) {
        sub[i] = strtok(NULL, ",:");
        removeExtras(sub[i]);
    }
    
    strcpy(image->title,sub[1]);
    strcpy(image->description,sub[3]);
    strcpy(image->namespace,"http://www.w3.org/2000/svg");

    /*Initialize all lists*/
    image->rectangles = initializeList(rectangleToString,deleteRectangle,compareRectangles);
    image->circles = initializeList(circleToString,deleteCircle,compareCircles);
    image->paths = initializeList(pathToString,deletePath,comparePaths);
    image->groups = initializeList(groupToString,deleteGroup,compareGroups);
    image->otherAttributes = initializeList(attributeToString,deleteAttribute,compareAttributes);
    
    free(string);
    return image;
}


/** Function to converting a JSON string into a Rectangle struct
*@pre JSON string is not NULL
*@post Rectangle has not been modified in any way
*@return A newly allocated and initialized Rectangle struct
*@param str - a pointer to a string
**/
Rectangle* JSONtoRect(const char* svgString) {
    char *sub[10];
    char *string = malloc(strlen(svgString));
    Rectangle* rect = malloc(sizeof(Rectangle));

    strcpy(string,svgString);
    removeExtras(string);

    sub[0] = strtok(string, ",:");
    for(int i = 1; i < 10; i++) {
        sub[i] = strtok(NULL, ",:");
        removeExtras(sub[i]);
    }
    
    rect->x = atof(sub[1]);
    rect->y = atof(sub[3]);
    rect->width = atof(sub[5]);
    rect->height = atof(sub[7]);
    
    if(strcmp(sub[9],"") != 0) {
        removeExtras(sub[9]);
        strcpy(rect->units,sub[9]);
    } else {
        strcpy(rect->units,"");
    }
    rect->otherAttributes = initializeList(rectangleToString,deleteRectangle,compareRectangles);

    return rect;
}

/** Function to converting a JSON string into a Circle struct
*@pre JSON string is not NULL
*@post Circle has not been modified in any way
*@return A newly allocated and initialized Circle struct
*@param str - a pointer to a string
**/
Circle* JSONtoCircle(const char* svgString) {
    char *sub[8];
    char *string = malloc(strlen(svgString));
    Circle* circ = malloc(sizeof(Circle));

    strcpy(string,svgString);
    removeExtras(string);
  
    sub[0] = strtok(string, ",:");
    for(int i = 1; i < 8; i++) {
        sub[i] = strtok(NULL, ",:");
        removeExtras(sub[i]);
    }
    
    circ->cx = atof(sub[1]);
    circ->cy = atof(sub[3]);
    circ->r = atof(sub[5]);
    if(strcmp(sub[7],"") != 0) {
        removeExtras(sub[7]);
        strcpy(circ->units,sub[7]);
    } else {
        strcpy(circ->units,"");
    }

    circ->otherAttributes = initializeList(circleToString,deleteCircle,compareCircles);
    return circ;
}

char *fileToImageToJson(char* fileName, char* schema) {                                
    SVGimage *img = createValidSVGimage(fileName,schema);
  
    if(img == NULL) {
        return "{}";
    } else if (validateSVGimage(img,schema) == false) {
        return "{}";
    }

    char *temp = SVGtoJSON(img);
    deleteSVGimage(img);
    return(temp);
}

char *title(char* fileName, char* schema){
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "";
    } else if (validateSVGimage(img,schema) == false) {
        return "";
    } else if(img->title[0] == '\0') {
        return " ";
    }

    char *temp = malloc(sizeof(char)*strlen(img->title) + 1);
    strcpy(temp,img->title);
    temp[strlen(img->title)] = '\0';
    deleteSVGimage(img);
    return(temp);
}

char *desc(char* fileName, char* schema){
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "";
    } else if (validateSVGimage(img,schema) == false) {
        return "";
    } else if(img->description[0] == '\0') {
        return " ";
    }

    char *temp = malloc(sizeof(char)*strlen(img->description) + 1);
    strcpy(temp,img->description);
    temp[strlen(img->description)] = '\0';
    deleteSVGimage(img);
    return(temp);
}

char* circJSON(char* fileName, char* schema) {
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "[{}]";
    } else if (validateSVGimage(img,schema) == false) {
        return "[{}]";
    } 
    List *temp = getCircles(img);
    char *string = circListToJSON(temp);
    safeFreeList(temp);
    deleteSVGimage(img);
    return(string);
}

char* rectJSON(char* fileName, char* schema) {
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "[{}]";
    } else if (validateSVGimage(img,schema) == false) {
        return "[{}]";
    } 
    List *temp = getRects(img);
    char *string = rectListToJSON(temp);
    safeFreeList(temp);
    deleteSVGimage(img);
    return(string);
}

char* pathJSON(char* fileName, char* schema) {
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "[{}]";
    } else if (validateSVGimage(img,schema) == false) {
        return "[{}]";
    } 
    List *temp = getPaths(img);
    char *string = pathListToJSON(temp);
    safeFreeList(temp);
    deleteSVGimage(img);
    return(string);
}

char* groupJSON(char* fileName, char* schema) {
    SVGimage *img = createValidSVGimage(fileName,schema);

    if(img == NULL) {
        return "[{}]";
    } else if (validateSVGimage(img,schema) == false) {
        return "[{}]";
    } 
    List *temp = getGroups(img);
    char *string = groupListToJSON(temp);
    safeFreeList(temp);
    deleteSVGimage(img);
    return(string);
}

void removeExtras(char *string) {
    for(int i = 0; i < strlen(string); i++) {
        string[i] = string[i + 1];
    }
    string[strlen(string) - 1] = '\0';
    return;
}

bool createSVGFileFromJSON(const char* svgString, char *fileName, char *schema) {
    SVGimage *img = JSONtoSVG(svgString);
    
    if(validateSVGimage(img,schema) == false) {
        return false;
    }

    if(strstr(fileName, ".svg") == NULL) {
        fileName = realloc(fileName,strlen(fileName)+4);
        strcat(fileName,".svg");
    }
    
    char *temp = malloc(strlen(fileName) + 10);
    strcpy(temp,"./uploads/");
    strcat(temp,fileName);
    writeSVGimage(img,temp);
    deleteSVGimage(img);
    return true;
}

bool addRectToSVG(const char* svgString, char *fileName, char *schema) {
    SVGimage *img = createValidSVGimage(fileName, schema);
    if(validateSVGimage(img,schema) == false) {
        return false;
    }
    Rectangle *rect = JSONtoRect(svgString);
    
    addComponent(img,RECT,rect);

    writeSVGimage(img,fileName);
    return true;
}

bool addCircleToSVG(const char* svgString, char *fileName, char *schema) {
    SVGimage *img = createValidSVGimage(fileName, schema);
    if(validateSVGimage(img,schema) == false) {
        return false;
    }
    Circle *circ = JSONtoCircle(svgString);
    
    addComponent(img,CIRC,circ);

    writeSVGimage(img,fileName);
    return true;
}

bool addAttrToSVG(const char* svgString, char *fileName, int row, char* schema) {
    SVGimage *img = createValidSVGimage(fileName, schema);
    
    if(validateSVGimage(img,schema) == false) {
        return false;
    }
    
    char *sub[4];
    char *string = malloc(strlen(svgString) + 1);
    Attribute* attr = malloc(sizeof(Attribute));
    strcpy(string,svgString);
    removeExtras(string);
  
    sub[0] = strtok(string, ",:");
    for(int i = 1; i < 4; i++) {
        sub[i] = strtok(NULL, ",:");
        removeExtras(sub[i]);
    }

    attr->name = malloc(sizeof(char)*strlen(sub[1]) + 1);
    strcpy(attr->name,sub[1]);
    attr->name[strlen(attr->name)] = '\0';
    attr->value = malloc(sizeof(char)*strlen(sub[3]) + 1);
    strcpy(attr->value,sub[3]);
    attr->value[strlen(attr->value)] = '\0';
    
    if(row <= getLength(img->rectangles)) {
        for(int i = 0; i <= getLength(img->rectangles); i++) {
            if(i == row) {
                setAttribute(img,RECT,i,attr);
                break;
            }
        }
    } else if(row <= getLength(img->rectangles) + getLength(img->circles)) {
        for(int i = 0; i <= getLength(img->circles); i++) {
            if(i == row) {
                setAttribute(img,CIRC,i,attr);
                break;
            }
        }
    } else if(row <= getLength(img->rectangles) + getLength(img->circles) + getLength(img->paths)) {
        for(int i = 0; i <= getLength(img->paths); i++) {
            if(i == row) {
                setAttribute(img,PATH,i,attr);
                break;
            }
        }
    } else {
        for(int i = 0; i <= getLength(img->groups); i++) {
            if(i == row) {
                setAttribute(img,GROUP,i,attr);
                break;
            }
        }
    }
    
    writeSVGimage(img,fileName);
    return true;
}

char *getAttributes(char*fileName, char* id, char *schema) {
    char *string;
    List *list;
    ListIterator iter;
    int size = 0;

    SVGimage *img = createValidSVGimage(fileName,schema);
    if(img == NULL) {
        return "{}";
    } else if (validateSVGimage(img,schema) == false) {
        return "{}";
    }

    size += 2;

    if(strcmp(id,"rectangle") == 0) {
        list = getRects(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                size += 1;
            }
            Rectangle *rect = nextElement(&iter);
            size += strlen(attrListToJSON(rect->otherAttributes));
        }
    } else if(strcmp(id,"circle") == 0) {
        list = getCircles(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                size += 1;
            }
            Circle *circle = nextElement(&iter);
            size += strlen(attrListToJSON(circle->otherAttributes));
        }
    } else if(strcmp(id,"path") == 0) {
        list = getPaths(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                size += 1;
            }
            Path *path = nextElement(&iter);
            size += strlen(attrListToJSON(path->otherAttributes));
        }
    } else if(strcmp(id,"group") == 0) {
        list = getGroups(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                size += 1;
            }
            Group *group = nextElement(&iter);
            size += strlen(attrListToJSON(group->otherAttributes));
        }
    }

    string = malloc(sizeof(char)*size + 1); 

    strcpy(string,"[");

    if(strcmp(id,"rectangle") == 0) {
        list = getRects(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                strcat(string,",");
            }
            Rectangle *rect = nextElement(&iter);
            strcat(string,attrListToJSON(rect->otherAttributes));
        }
    } else if(strcmp(id,"circle") == 0) {
        list = getCircles(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                strcat(string,",");
            }
            Circle *circle = nextElement(&iter);
            strcat(string,attrListToJSON(circle->otherAttributes));
        }
    } else if(strcmp(id,"path") == 0) {
        list = getPaths(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                strcat(string,",");
            }
            Path *path = nextElement(&iter);
            strcat(string,attrListToJSON(path->otherAttributes));
        }
    } else if(strcmp(id,"group") == 0) {
        list = getGroups(img);
        iter = createIterator(list);
        for(int i = 0; i < getLength(list); i++) {
            if(i > 0){
                strcat(string,",");
            }
            Group *group = nextElement(&iter);
            strcat(string,attrListToJSON(group->otherAttributes));
        }
    }
    strcat(string,"]");
    string[size] = '\0';

    deleteSVGimage(img);
    return string;
} 
