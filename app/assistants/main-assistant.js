/*
 * bootstrap the program.  
 * You need to have a MainAssistant.
 */

function MainAssistant() {
	this.dao = new Database;
}

MainAssistant.prototype.setup = function() {
	/* init the plugin stuff */
	var pluginObj = window.document.createElement("object");
    	pluginObj.id = "dbPlugin";
	pluginObj.type = "application/x-palm-remote";
	pluginObj.width = 0;
	pluginObj.height = 0;
	pluginObj['x-palm-pass-event'] = true;

	var param1 = window.document.createElement("param");
    	param1.name = "appid";
	param1.value = "org.webos-internals.db";
    
	var param2 = window.document.createElement("param");
	param2.name = "exe";
	param2.value = "db";
	
	pluginObj.appendChild(param1);
	pluginObj.appendChild(param2);
    
	df = window.document.createDocumentFragment();
	df.appendChild(pluginObj);
	this.controller.stageController.window.document.body.appendChild(df);
	var plugin = this.controller.stageController.get("dbPlugin");
	this.dao.plugin = plugin;
	/* end of plugin stuff */
	
	this.controller.setupWidget('list',
		{
			itemTemplate:'main/list-item',
			itemsCallback: this.onItemsCallback.bind(this)
		}, 
		{ }
	);
}

MainAssistant.prototype.cleanup = function() {
}

MainAssistant.prototype.onItemsCallback = function(widget, offset, limit) {
	this.dao.selectItems(function(rows) {
		Mojo.Log.error("onItemCallback.onCompletedCallback: %s", rows.length);
		widget.mojo.noticeUpdatedItems(offset, rows);
		widget.mojo.setLength(rows.length);
	});
}