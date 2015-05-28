#include <iostream>
#include "BTreeNode.h"
#include "PageFile.h"
using namespace std;

void printNodeContents(BTLeafNode& node)
{
  int key;
  RecordId rid;

  for(int eid = 0; eid < node.getKeyCount(); eid++)
  {
    node.readEntry(eid,key,rid);
    cout << "Key: " << key
         << " Rid: (" << rid.pid << "," << rid.sid << ")" << endl;
  }
}

int main ()
{
  PageFile pf;
  BTLeafNode node;
  BTLeafNode node2;
  RecordId rid;
  RecordId rid2;
  int key;
  int eid;

  if (pf.open("roman.idx",'w'))
  {
    cout << "Could not open page file" << endl;
    return 1;
  }

  rid.pid = 1;
  

  for (int i = 0; i < 50; i++)
  {
    rid.sid = i*10;
    node.insert(i*2,rid);
  }

  cout<<"Hi, there!"<<endl;

  node.setNextNodePtr(31);

  key = node.getNextNodePtr();

  cout << "Next Node Ptr: " << key << endl;

  printNodeContents(node); 

  rid2.pid = 2;
  rid2.sid = 222;
  int siblingKey = 0;
  node.insertAndSplit(11,rid2,node2,siblingKey);
  cout << "siblingKey: "<<siblingKey<<endl;
  printNodeContents(node);
  cout<<"------------"<<endl;
  printNodeContents(node2);
}
