<@setoutput path=targetMainFile?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>
<#if gateName==""><#assign gateName = "g"></#if>
<#assign gateName = StringUtils.makeValidIdentifier(gateName)>

<#assign nodeTypeExists = NedUtils.isVisibleType(nodeType,targetFolder)>
<#if nodeTypeExists>
import ${nodeType};
  <#assign nodeType = nodeType?replace("^.*\\.", "", "r")>
<#else>
  <#assign nodeType = StringUtils.makeValidIdentifier(nodeType)>
</#if>

<#assign channelTypeSupplied = channelType!="">
<#assign channelTypeExists = channelTypeSupplied && NedUtils.isVisibleType(channelType,targetFolder)>
<#if channelTypeExists>
import ${channelType};
  <#assign channelType = channelType?replace("^.*\\.", "", "r")>
<#else>
  <#assign channelType = StringUtils.makeValidIdentifier(channelType)>
</#if>

<#if !nodeTypeExists>
module ${nodeType} {
    parameters:
        @display("i=abstract/router_s");
    gates:
        inout ${gateName}[];
    connections allowunconnected:
}
</#if>

<#if channelTypeSupplied && !channelTypeExists>
channel ${channelType} extends ned.DatarateChannel {
    parameters:
        int cost = default(0);
}
</#if>

<#-- read links file and collect nodes from it -->
<#assign linksData = FileUtils.readCSVFile(linksFile, linksFileIgnoreFirstLine, true, true)>
<#assign nodes = LangUtils.newMap()>
<#list linksData as link>
  <#if (link?size >= 2)>
    <@do nodes.put(link[0],"") !/>
    <@do nodes.put(link[1],"") !/>
  </#if>
</#list>

<#-- read nodes file and store x,y coordinates -->
<#if nodesFile != "">
  <#assign nodesData = FileUtils.readCSVFile(nodesFile, nodesFileIgnoreFirstLine, true, true)>
  <#list nodesData as node>
  <#if (node?size == 3)>
    <@do nodes.put(node[0], node[1]+","+node[2]) !/>
  <#else>
    <#stop "Unexpected number of items on line >>>" + LangUtils.toString(node) + "<<< (expected 2, 3, or 4)">
  </#if>
</#list>
</#if>
//
// Network generated from ${linksFile} <#if nodesFile!="">and ${nodesFile}</#if>
//
<#if wizardType=="compoundmodule"><#assign keyword="module"><#else><#assign keyword="network"></#if>
${keyword} ${targetTypeName} {
    parameters:
        int count = 0;
        int number_of_node = ${linksData[0][0]};
        double PROB_0 = default(0.5);
    
    
    submodules:
<#list nodes.keySet().toArray()?sort as node>
  <#if nodes[node]=="">
        ${node}: ${nodeType};
  <#else>
        ${node}: ${nodeType} { @display("p=${nodes[node]}"); }
  </#if>
</#list>
    connections:
<#list linksData as link>
	<#if (link?size == 2)>
        ${link[0]}.${gateName}++ <--> { @display("ls=green,3"); } <--> ${link[1]}.${gateName}++;
  	</#if>
</#list>
}

