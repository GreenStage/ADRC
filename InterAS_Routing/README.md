# InterAS_Routing

Functions:

```

network_parse_connected(index,type){
    index.visited := true;
    for every link of type:
        if(link.node.visited == false):
            node.onStack := true
            network_parse_connected(link.node)
            node.onStack := false
        else if(node.onStack):
            network.revisitAttempt := true
        end if
    end for
}

network_ensure_no_costumer_cycle(){
    for every node not visited:
        node.onStack := true
        network_parse_connected(node,COSTUMER)
        node.onStack := false;
        if(network.revisitAttempt)
            return false
    end for
    return true
}

network_check_commercial(){
    bool is_commercial := true;
    for every node1 in network.top_providers:
        for every node2 in network.top_provider:
            if node.links.PEER.contains(node2)
                matrix[node1][node2] := true
            endif
        end for
    end for

    for i : matrix.size:
        for j: matrix.size
            if i != j and matrix[i][j] == false:
                is_commercial = false;
                break;
            end if
        end for
    end for

    if is_commercial
        return;
    end if

    else if user asked to make it commercial:
    for i : matrix.size:
        for j: matrix.size
            if i != j and matrix[i][j] == false:
                create peer link between i and j
            end if
        end for
    end for
}

network_update_dest_route(node){
    new_route.nHops := node.route.nHops + 1
    new_route.advertise := node

    speedCalc := true if network is commercial and user
                 wants to calculate only the type of routes

    for every node.links.type.neighbor:
        new_route.type = INVERSE(type)

        if links.type.neighbor.route is worse then new_route
           or links.type.neighbor.route.advertiser == node:

            if node.route.type == COSTUMER or route.isDestination 
               or (node.links.type == COSTUMER AND speedCalc:

                node.neighbor.route := new_route
                nodes.not_visited.add(node.links.type.neighbor)
                nodes.not_visited.fixHeapUp
            end if
        end if
    end for
    
}

network_find_paths_to(destination){
    if network.isCommercial and network.calctype = TYPE_ONLY
        initialize all routes type to PROVIDER
    end if

    nodes.not_visited[0] := destination
    for node : nodes.not_visited:
        network_update_dest_route(node)
        nodes.not_visited.remove(node)
        nodes.not_visited.fixHeapDown
    end for

}

network_parse_all(type){
    for node : network.nodes:

        if (network.isCommercial and node.hasOnlyOneLink) 
            or (node.hasOnlyOneLink and node.onlyLinkType == PROVIDER):

             continue
        end if

        temporary_stats = network_find_paths_to(node)

        for each node.link.peer:

            if network.isCommercial and node.link.peer.hasOnlyOneLink:
                statistics += calc_from_neighbor(temporary_stats)

            else if node.link.peer.hasOnlyOneLink and 
                    node.link.peer.onlyLinkType == PROVIDER:

                statistics += calc_from_neighbor(temporary_stats)
            end if

        end for
        statistics += temporary_stats
    end for

    return statistics
}

```
