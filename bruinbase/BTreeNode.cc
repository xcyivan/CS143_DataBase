#include "BTreeNode.h"

using namespace std;

BTLeafNode::BTLeafNode(){
	memset(buffer,0,PageFile::PAGE_SIZE);
	Capacity = (PageFile::PAGE_SIZE-sizeof(PageId)-sizeof(int))/sizeof(ElementLeaf);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf){
	RC rc;
	rc = pf.read(pid,buffer);
	return rc; 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf){
	RC rc;
	rc = pf.write(pid,buffer);
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount(){
	int count = 0;
	ElementLeaf* element = (ElementLeaf*) buffer;
	while(count<Capacity && element->key!=0){
		count++;
		element++;
	}
	return count;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid){
	if(getKeyCount()>=Capacity){
		// cout<<"Can't insert into BTLeafNode, the node is full!"<<endl;
		return -1;// the node is full
	}
	int insertId;
	if(locate(key,insertId) == 0){
		cout<<"Error! Inserting duplicate value into BTLeafNode!"<<endl;
		return -1;
	}
	ElementLeaf* insertEle = (ElementLeaf*) buffer + insertId;
	ElementLeaf* cursorEle = (ElementLeaf*) buffer + getKeyCount();
	while(insertEle != cursorEle){
		ElementLeaf* preEle = cursorEle-1;
		*cursorEle = *preEle;
		cursorEle = preEle;
	}
	insertEle->key = key;
	insertEle->rid = rid;
	return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey){
	int siblingId = (getKeyCount()+1)/2;
	int insertId;// the entry id for the inserted entry
	if(locate(key,insertId) == 0){
		cout<<"Error in BTLeafNode::insertAndSplit, duplicate entry"<<endl;
		return -1;
	}
	if(insertId>=siblingId){
		siblingKey = ((ElementLeaf*)buffer+siblingId)->key;
		sibling.insert(key,rid);
	}
	else{
		ElementLeaf* insertEle = (ElementLeaf*)buffer+insertId;
		ElementLeaf* cursorEle = (ElementLeaf*)buffer+siblingId-1;
		siblingKey = cursorEle ->key;
		sibling.insert(cursorEle->key,cursorEle->rid);
		while(cursorEle!=insertEle){
			ElementLeaf* preEle = cursorEle -1;
			*cursorEle = * preEle;
			cursorEle = preEle;
		}
		insertEle->key = key;
		insertEle->rid = rid;
	}

	int count = 0;
	for(int i=siblingId; i<getKeyCount(); i++){
		ElementLeaf* element = (ElementLeaf*) buffer + i;
		sibling.insert(element->key,element->rid);
		count++;
	}
	//clear memory for removed data
	ElementLeaf* element = (ElementLeaf*) buffer + siblingId;
	memset(element,0,sizeof(ElementLeaf)*count);
	return 0;
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid){
	eid = 0;
	ElementLeaf* element = (ElementLeaf*) buffer + eid;
	while(eid<getKeyCount()){
		if(element->key == searchKey)
			return 0;
		if(element->key > searchKey)
			return RC_NO_SUCH_RECORD;
		eid++;
		element++;
	}
	return RC_NO_SUCH_RECORD;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid){
	if(eid<0 || eid>=getKeyCount()){
		cout<<"Error when readEntry, eid excess boundary!"<<endl;
		return -1;
	}
	ElementLeaf* element = (ElementLeaf*) buffer+eid;
	key = element->key;
	rid = element->rid;
	return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr(){
	PageId* pid = (PageId *)(buffer+PageFile::PAGE_SIZE) - 1;
	return *pid;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid){
	PageId* ppid = (PageId *)(buffer+PageFile::PAGE_SIZE) - 1;
	*ppid = pid;
	return 0;
}







//reserve a slot for an entry, which would be used in insertAndSplit()
BTNonLeafNode::BTNonLeafNode(){
	memset(buffer,0,PageFile::PAGE_SIZE);
	Capacity = (PageFile::PAGE_SIZE-sizeof(PageId)-sizeof(int)-sizeof(ElementNonLeaf))/sizeof(ElementNonLeaf);

}

void BTNonLeafNode::showEntries() {
    int nKeys = getKeyCount();
    int i = 0;
    for (ElementNonLeaf* e = (ElementNonLeaf*)(buffer); i < nKeys + 1; ++e, ++i) {
        printf("entry #%i: {key: %i, pid: %i}\n", i, e->key, e->pid);
    }
}

void BTNonLeafNode::showEntriesWithFirstPageId(){
    PageId* ptr = (PageId *)buffer;
    printf("\nFirst pid: %i \n",*ptr);
    showEntries();
}


// /*
//  * Read the content of the node from the page pid in the PageFile pf.
//  * @param pid[IN] the PageId to read
//  * @param pf[IN] PageFile to read from
//  * @return 0 if successful. Return an error code if there is an error.
//  */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf){
	RC rc;
	rc = pf.read(pid,buffer);
	return rc;
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf){
	RC rc;
	rc = pf.write(pid, buffer);
	return rc;
}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount(){
	int count = 0;
	ElementNonLeaf* element = (ElementNonLeaf*) (buffer);
	while(count<Capacity && element->key!=0){
		count++;
		element++;
	}
	return count;
}

/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */

RC BTNonLeafNode::insert(int key, PageId pid){
	if(getKeyCount()>=Capacity)
		return -1;
	ElementNonLeaf* element = (ElementNonLeaf*) (buffer);
	int index=0;
	for(; index<getKeyCount();index++){
		if(element[index].key == key){
			cout<<"Warning! inserting duplicate value in BTNonLeafNode!"<<endl;
			return -1;
		}
		if(element[index].key > key){
			break;
		}
	}
	for(int i=getKeyCount();i>index;i--){
		ElementNonLeaf* entry = (ElementNonLeaf*) (buffer) + i;
		*entry = *(entry-1); 
	}
	element[index].key = key;
	element[index].pid = pid;
	return 0;
}


/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */


RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey){
	//same as above except for size check
	ElementNonLeaf* element = (ElementNonLeaf*) (buffer);
	int index=0;
	for(; index<getKeyCount();index++){
		if(element[index].key == key){
			cout<<"Warning! inserting duplicate value in BTNonLeafNode!"<<endl;
			return -1;
		}
		if(element[index].key > key){
			break;
		}
	}
	for(int i=getKeyCount();i>index;i--){
		ElementNonLeaf* entry = (ElementNonLeaf*) (buffer) + i;
		*entry = *(entry-1); 
	}
	element[index].key = key;
	element[index].pid = pid;
	//-----end of same code -----

	int splitIdx = getKeyCount()/2;
	ElementNonLeaf* splitEntry = (ElementNonLeaf*) (buffer) + splitIdx;
	midKey = splitEntry->key;
	sibling.initializeRoot(splitEntry->pid, (splitEntry+1)->key, (splitEntry+1)->pid);
	memset(splitEntry,0,sizeof(ElementNonLeaf));
	for(int i=splitIdx+1; i<getKeyCount(); i++){
		ElementNonLeaf* ele = (ElementNonLeaf*)(buffer)+i;
		sibling.insert(ele->key, ele->pid);
		memset(ele, 0, sizeof(ElementNonLeaf));
	}
	return 0;
}

/*
 * Given the searchKey, find the entry number and
 * references it in eid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param eid[OUT] the entry number with the pointer to follow
 * @return 0 if successful. Return an error code if there is an error.
 */

RC BTNonLeafNode::locateChildPtr(int searchKey, int& eid){
	eid = getKeyCount()-1;
	while(eid>=0){
		ElementNonLeaf* element = (ElementNonLeaf*) buffer + eid;
		if(element->key<searchKey)
			return 0;
		else
			eid--;
	}
	return 1;
}

/*
 * Read the (key, pid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, pid) pair from
 * @param key[OUT] the key from the entry
 * @param pid[OUT] the PageId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */

RC BTNonLeafNode::readEntry(int eid, PageId& pid){
	if(eid>=getKeyCount())
		return -1;
	if(eid<0){
		PageId *ptr = (PageId *)(buffer+PageFile::PAGE_SIZE-sizeof(PageId));
		pid = *ptr;	
	}
	else{
		ElementNonLeaf* element = (ElementNonLeaf*) buffer + eid;
		pid = element->pid;
	}
	return 0;
}


/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2){
	memset(buffer,0,PageFile::PAGE_SIZE);
	PageId* ptr = (PageId*) (buffer+PageFile::PAGE_SIZE-sizeof(PageId));
	*ptr = pid1;
	ElementNonLeaf* element = (ElementNonLeaf*) buffer;
	element->key = key;
	element->pid = pid2;
	return 0;
}
