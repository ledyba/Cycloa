var cycloa;
if(!cycloa) cycloa={};

cycloa.NesController = function(module){
	this.module_ = module;
	this.zoomed_ = false;
};
cycloa.NesController.prototype.load = function(data){
	this.module_.postMessage(data);
	this.start();
};
cycloa.NesController.prototype.start = function(){
	this.module_.postMessage("start");
};
cycloa.NesController.prototype.stop = function(){
	this.module_.postMessage("stop");
};
cycloa.NesController.prototype.hardReset = function(){
	this.module_.postMessage("hardReset");
};
cycloa.NesController.prototype.reset = function(){
	this.module_.postMessage("reset");
};
cycloa.NesController.prototype.zoom = function(){
	if(this.zoomed_){
		$("#nes_nacl").animate({width: 256, height: 240});
	}else{
		$("#nes_nacl").animate({width: 512, height: 480});
	}
	this.zoomed_ = !this.zoomed_;
}


cycloa.log = function(){
	var msg = "";
	for(var i=0;i<arguments.length; ++i){
		msg += arguments[i];
	}
	console.log(msg);
	$("#state").text(msg);
};

cycloa.moduleLoaded = function(){
	cycloa.log("module loaded");
	jQuery.event.props.push('dataTransfer');
	 $("html").bind("drop", function(e){
		e.stopPropagation();
		e.preventDefault();
		var file = e.dataTransfer.files[0];
		
		$("#state").text("Now loading...");
		var reader = new FileReader();
		reader.onload = function (dat) {
			cycloa.nesController.load(dat.target.result);
			$("#state").text("done.");
		};
		reader.readAsArrayBuffer(file);
	});
	$("html").bind("dragenter dragover", false);

	$("#rom_sel").bind("change", function(e){
		var val = e.currentTarget.value;
		if(val){
			$("#state").text("Now loading...");
			var xhr = jQuery.ajaxSettings.xhr();
			xhr.open('GET', val, true);
			xhr.responseType = 'arraybuffer';
			xhr.onreadystatechange = function() {
				if (this.readyState === this.DONE) {
					if(this.status === 200){
						cycloa.nesController.load(this.response);
					}else{
						$("#state").text("oops. Failed to load game... Status: "+this.status);
					}
				}
			};
			xhr.send();
		}
	});
	cycloa.nesController = new cycloa.NesController($("#nes_nacl")[0]);
};

cycloa.handleMessage = function(msg){
	var payload = msg.data;
	cycloa.log(payload);
};

(function(){
	$(document).ready(function(){
		cycloa.log("page loaded.");
		// モジュールのロード処理。これをつけないと、かなりの確率でロードに失敗してしまうみたい。
		// Googleのサンプルでもそうなっております
		var embed = $('<embed>');
		embed.attr('name','nacl_module');
		embed.attr('id', 'nes_nacl');
		embed.attr('width', '256');
		embed.attr('height', '240');
		embed.attr('type', 'application/x-nacl');
		embed.attr('src', './Cycloa.nmf');
		$('#nes_screen').append(embed);
		
		// モジュールがロードされた時の処理
		$("#nes_screen")[0].addEventListener('load', cycloa.moduleLoaded, true);
		$("#nes_screen")[0].addEventListener('message', cycloa.handleMessage, true);
		// ハードリセット
		$("#nes_hardreset").bind("click", function(){
			cycloa.nesController.hardReset();
		});
		// リセット
		$("#nes_reset").bind("click", function(){
			cycloa.nesController.reset();
		});
		// スタートとストップの切り替え
		$("#nes_stop").bind("click", function(){
			if(cycloa.nesController.stop()){
				$("#nes_start").removeClass("disable");
				$("#nes_stop").addClass("disable");
			}
		});
		$("#nes_start").bind("click", function(){
			if(cycloa.nesController.start()){
				$("#nes_stop").removeClass("disable");
				$("#nes_start").addClass("disable");
			}
		});
		// スクリーンズーム機能
		$("#screen_zoom").bind("click", function(){
			cycloa.nesController.zoom();
		});
		// ROMセレクタをデフォルトにする（Firefoxで必要）
		$("#rom_sel")[0].selectedIndex  = 0;
		$("#nes_stop").removeClass("disable");
		$("#nes_start").addClass("disable");
		
	});
}());

