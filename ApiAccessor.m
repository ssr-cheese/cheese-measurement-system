classdef ApiAccessor
    properties
        baseURI = matlab.net.URI('https://script.google.com/macros/s/AKfycbyep2Nb-uHKRhfZ0YCZOKeJETFCAK4nIwdh9UDnehVsfmJ1xkU/exec');
        type;
    end
    
    methods
        function obj = ApiAccessor(type)
            if nargin == 1
                obj.type = type;
            else
                obj.type = "";
            end
        end
        
        %出走順が{id}の人の詳細な情報を入手
        %返り値は machine: 機体名, prod: 氏名, org: 団体名
        function [machine, prod, org] = getDetail(obj, id) 
            import matlab.net.http.*
            import matlab.net.*
            
            req = RequestMessage;
            req.Method = RequestMethod.GET;
            
            uri = obj.baseURI;
            uri.Query = QueryParameter("id", id, "type", obj.type);
            
            res = send(req, uri);
            data = res.Body.Data;
            
            machine = data.machine;
            prod = data.producer;
            org = data.organization;
        end
        
        %1~{num}までの順位を取得
        function rankArray = getRank(obj, num)
            import matlab.net.http.*
            import matlab.net.*
            
            req = RequestMessage;
            req.Method = RequestMethod.GET;
            
            uri = obj.baseURI;
            uri.Query = QueryParameter("rank", num, "type", obj.type);
            
            res = send(req, uri);
            rankArray = res.Body.Data.rank;
        end
        
        %タイムを更新(id:出走順,nth:n走目)
        %timeは数値、文字列どちらでも可
        function updateTime(obj, id, nth, time) 
            import matlab.net.http.*
            import matlab.net.*
            
            req = RequestMessage;
            req.Method = RequestMethod.POST;
            jsonStr = jsonencode(struct("id",id,"nth",nth,"time",time, "type", obj.type));
            req.Body = MessageBody(jsonStr);
            
            send(req, obj.baseURI);
        end
    end
end