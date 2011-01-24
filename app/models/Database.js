/*
 * Main Database file. 
 */
var Database = Class.create({
	/* ctor */
	initialize: function() {
	
		this.plugin = undefined;
		this.dbname = "foobar.db";
		this.db = null;
	},
	/*
	 * open the database
	 */
	openDatabase : function(callback) {
		if (this.db != null && this.db['open'] == 1) {
			return;
		}

		var f = (function() {

			if (this.plugin.openDatabase == null) {
				Mojo.Log.error("-------plugin not set, waiting to try again");
				setTimeout(f, 250);
			} else {
				var ret = this.plugin.openDatabase(this.dbname);
				var json = Mojo.parseJSON(ret);
				Mojo.Log.error("openDatabase: %j", json);
				this.db = json;	
				callback();
			}
		}).bind(this);

		f();
	},

	closeDbIfOpen: function() {
		if (this.db != null && this.db['open'] == 1) {
			var ret = this.plugin.closeDatabase(this.dbname);
			var json = Mojo.parseJSON(ret);
			this.db = null;
		}
	},
	
	selectItems : function(onCompletedCallback) {
		var callback = function() {
			var ret = this.plugin.executeSql(this.dbname, "select content from foo");
			Mojo.Log.error("selectItems: %s", ret);
			var json = Mojo.parseJSON(ret);
			var rows = $A();
			for (var i = 0; i < json.rows.length; i++) {
				rows[i] = json.rows[i]	
			}
			onCompletedCallback(rows);
		};
		this.openDatabase(callback.bind(this));
	}
});
