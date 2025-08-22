# KiCad华秋发行版Component协议及从WebView摆放器件的流程


## Component 协议

```ts
export interface SymbolResource {
  uri: string;
}

export interface ModelResource {
  uri: string;
}

export interface FootprintResource {
  uri?: string;
  models: ModelResource[];
}

  export interface Attribute {
    name: string;
    value: string;
  }

export interface ComponentResource {
  component_name?: string;
  symbol_resource: SymbolResource;
  footprint_resources: FootprintResource[];
  attributes: Attribute[];
}

```

示例协议内容 :

```json

{
    "action": "place_symbol_resource",
    "context": {
        "component": {
            "attributes": [
                {
                    "name": "MPN",
                    "value": "LM317"
                },
                {
                    "name": "Datasheet",
                    "value": "http://file.elecfans.com/web2/M00/72/BF/pYYBAGNTjWGAdACFAAx89VwnAvU953.pdf"
                },
                {
                    "name": "Value",
                    "value": "LM317"
                },
                {
                    "name": "Manufacturer",
                    "value": "JSCJ"
                },
                {
                    "name": "Description",
                    "value": "输出类型:可调;输出极性:正;输出通道数:1;最大输入电压:40V;输出电压:1.2V~37V;压差:-;输出电流:2.2A;电源纹波抑制比(PSRR):75dB@(120Hz);"
                },
                {
                    "name": "MFG",
                    "value": "JSCJ"
                },
                {
                    "name": "output_type",
                    "value": "可调"
                },
                {
                    "name": "number_of_outputs",
                    "value": "1"
                }
            ],
            "component_name": "LM317",
            "footprint_resources": [
                {
                    "models": [],
                    "uri": "https://www.eda.cn/all/2/98be2a8fee534be798c907763165fb1d.kicad_mod"
                }
            ],
            "symbol_resource": {
                "uri": "https://www.eda.cn/all/2/4213143937943038262.kicad_sym"
            }
        },
        "option": {
            "place_all_units": true,
            "place_repeated_copies": false
        }
    }
}

```



## 从WebView摆放器件的流程

从WebView中摆放器件到原理图的流程如下：

1. 用户点击摆放按钮

2. 将上述Component 协议通过PostMessage发给父级窗体(eda.cn)

3. 父级网页如eda.cn是会监听iframe的消息、并将消息转发给KiCad客户端
    
4. KiCad会接前述Component 协议协议定义的器件，并摆放符号至原理图编辑器、同时更新SymbolLibTable及FootprintLibTable


## 注意事项

子窗体使用PostMessage往父窗体发消息时，须显示指定父窗体的域名、若不确定父窗体的域名，也可以为为"*"

```js

window.parent.postMessage(YOUR_MESSAGE, "*");

```
