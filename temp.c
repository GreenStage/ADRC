void PrintTable(Tree * root){
    memset(lines,0,MAX_TREE_HEIGHT * sizeof (char*));
    printR(Tree)
}

{
    printf("%d",ptr->hopNumber);

}


Tree * DeletePrefix(Tree * prefixTree, char * address){
    static int i = 0;
    //int retval;
    static Tree_Node * it = NULL;
    Tree_Node * aux = NULL;
    //Tree_Node * ptr;
  
    //pra que raio usas isto?
    ASSERT_RETR(i > (int) strlen(address) - 1, -1);
  
    if(it == NULL){
      it = prefixTree->root;
    } 
  

    for(i = 0; i < (int) strlen(address); i++){

        aux = it;
        
        switch(address[i]){
            case '0':
            if(!it->left){
                //calhou coco return it->nextHop;
            }
            it= it->left;
            break;
            case '1':
            if(!it->right){
                //calhou coco return it->nextHop;
            }
            it= it->right;
            break;
            default:
            return NULL;
        
        }
    
    }
    //it now points to the one that shall be removed
    if(!it->left && !it->right){
        //this node has no children, it can be FREE

        if(aux->left == it){
            aux->left = NULL;
        }else{
            aux->right = NULL;
        }

        free(it);//CHECK

    }else{
        //this node has some children, it needs to be here for them
        it->nextHop = 0;
    }
    
  
    return prefixTree;
}  

Tree * makeTreeEven(Tree * prefixTree){


    

    Tree * auxNode = prefixTree;

    



}