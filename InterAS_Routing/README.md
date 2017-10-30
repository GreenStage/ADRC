#InterAS_Routing

Functions:

    visit_costumers(node){
        if node.visited == false:
            node.visited := true
            for each costumer node has:
                visit_costumers(costumer)
            end for
        end if
    }

    network_check_commercial(){

        node := network.some_node;

        while node.has_providers
            visit_costumer(node)
            node := node.provider[0]
        end while

        visit_costumer(node)

        for each peer node has:
            visit_costumers[peer]
        end for

        if there are still nodes to visit:
            network is not commercial
        else
            network is commercial
        end if

        return
    }
