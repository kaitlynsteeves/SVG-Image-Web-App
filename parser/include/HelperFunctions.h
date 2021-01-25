/*  CIS2750
*   Kaitlyn Steeves
*   1040449
*   HelperFunctions.h
*/

//A1
int checkFileName( char* fileName );
void getNameSpace( xmlNode* head, SVGimage *img );
void getElements( xmlNode* head, SVGimage * image );
void getRectangleAttributes(xmlNode *ptr, Rectangle *rectangle);
void getCircleAttributes(xmlNode *ptr, Circle *circle);
void getPathAttributes(xmlNode *ptr, Path *path);
void getGroupAttributes(xmlNode *head, Group *group);
void getOtherAttributes(xmlNode *ptr, List *list);
char* getUnits(char *string);
void getSubgroup(Group *group, List* list);
float getRectArea(Rectangle *rect);
float getCircleArea(Circle *circle);
int getGroupLength(Group *group);
int numAttributes(List *list, char* type);

//A2
int checkDoc(xmlDoc *doc, char* schemaFile);
xmlDoc* createXmlDoc(SVGimage *img);
int checkSchemaName( char* fileName );
void addRectToDoc(xmlNode *root, Rectangle *rect);
void addCircleToDoc(xmlNode *root, Circle *circle);
void addPathToDoc(xmlNode *root, Path *path);
void addGroupToDoc(xmlNode *root, Group *group);
void safeFreeList(List *list);
int validateUnits(char units[50]);
int validateAttributes(List *list);

//A3
char *fileToImageToJson(char* fileName, char* schema);
char *title(char* fileName, char* schema);
char *desc(char* fileName, char* schema);
char* circJSON(char* fileName, char* schema);
char* rectJSON(char* fileName, char* schema);
char* pathJSON(char* fileName, char* schema);
char* groupJSON(char* fileName, char* schema);
void removeExtras(char *string);
bool createSVGFileFromJSON(const char* svgString, char *fileName, char* schema);
bool addRectToSVG(const char* svgString, char *fileName, char* schema);
bool addCircleToSVG(const char* svgString, char *fileName, char* schema);
bool addAttrToSVG(const char* svgString, char *fileName, int row, char* schema);
char *getAttributes(char*fileName, char* id, char* schema);


