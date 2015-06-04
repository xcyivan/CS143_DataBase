/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode){
    if(pf.open(indexname,mode)){
    	return -1;
    }
    char meta[PageFile::PAGE_SIZE];
    //new file
    if(pf.endPid()==0){
    	rootPid=-1;
    	treeHeight=0;

    	if(pf.write(0,meta)){
    		return -2;
    	}
    }
    //existing file, read meta data from pid 0
    else{
    	if(pf.read(0,meta)){
    		return -2;
    	}
    	rootPid = *((PageId*)meta);
    	treeHeight = *((int*)(meta+sizeof(PageId)));
    }
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close(){
    char meta[PageFile::PAGE_SIZE];
    *((PageId*)meta) = rootPid;
    *((int*)(meta+sizeof(PageId))) = treeHeight;
    if(pf.write(0,meta))
    	return -1;
    return pf.close();
}

RC BTreeIndex::insertHelper(int key, const RecordId& rid, PageId pid, int height, int& newKey, PageId& newPid){
	newKey=-1;
	//at leaf node, base case
	if(height == treeHeight){
		BTLeafNode leaf;
		leaf.read(pid, pf);
		if(leaf.insert(key,rid)){//overflow, use insert and split
			BTLeafNode newLeaf;
			if(leaf.insertAndSplit(key,rid,newLeaf,newKey)){
				return -1;
			}
		newPid = pf.endPid();
		newLeaf.setNextNodePtr(leaf.getNextNodePtr());
		leaf.setNextNodePtr(newPid);
		if(newLeaf.write(newPid,pf))
			return -2;
		}
		if(leaf.write(pid,pf))
			return -3;
	}
	//at non leaf node
	else{
		BTNonLeafNode nonleaf;
		int eid;
		PageId childPid;

		nonleaf.read(pid, pf);
		nonleaf.locateChildPtr(key, eid);
		nonleaf.readEntry(eid,childPid);
		insertHelper(key,rid,childPid,height+1,newKey,newPid);
		if(newKey>0){//child node overflow
			if(nonleaf.insert(newKey,newPid)){
				int midKey;
				BTNonLeafNode sibling;
				//if this node is full, split it
				if(nonleaf.insertAndSplit(newKey,newPid,sibling,midKey)){
					return -3;
				}
				newKey = midKey;
				newPid = pf.endPid();
				if(sibling.write(newPid,pf))
					return -4;
			}
			else{
				newKey=-1;
			}
			nonleaf.write(pid,pf);
		}
	}
	return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid){
	int newKey;
	PageId newPid;

	if(treeHeight==0){
		BTLeafNode leaf;
		leaf.insert(key,rid);
		rootPid = pf.endPid();
		treeHeight = 1;
		leaf.write(rootPid,pf);
		return 0;
	}
	if (insertHelper(key,rid,rootPid,1,newKey,newPid))
		return -1;

	if (newKey>0){
		BTNonLeafNode newRoot;
    	newRoot.initializeRoot(rootPid, newKey, newPid);
    	rootPid = pf.endPid();
    	treeHeight++;
    	newRoot.write(rootPid, pf);
	}
    return 0;
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor){
    PageId pid = rootPid;
    //go down with non leaf nodes
    for (int i=0; i<treeHeight-1; i++){
    	int eid;
    	BTNonLeafNode nonleaf;
    	nonleaf.read(pid,pf);
    	nonleaf.locateChildPtr(searchKey,eid);
    	nonleaf.readEntry(eid,pid);
    }
    //reach the leaf level
    BTLeafNode leaf;
    leaf.read(pid,pf);
    cursor.pid = pid;
    leaf.locate(searchKey,cursor.eid);
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid){
    BTLeafNode leaf;
    if(leaf.read(cursor.pid,pf))
    	return -1;
    if(leaf.readEntry(cursor.eid,key,rid))
    	return -2;

    if (cursor.pid <= 0 || cursor.pid >= pf.endPid())
    	return 1;

    cursor.eid++;
    if(cursor.eid>=leaf.getKeyCount()){
    	cursor.pid=leaf.getNextNodePtr();
    	cursor.eid=0;
    }
    return 0;
}
