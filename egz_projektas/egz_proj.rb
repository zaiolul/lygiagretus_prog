#!/usr/bin/ruby

=begin
------------REZULTATAI------------

Procesorius: AMD Ryzen 5 5600 (6 branduoliai 12 gijų)
Programos laikas su 1 darbininku: 18,61 s
Programos laikas su 12 aktorių: 2,93 s

Laikas matuotas naudojantis GNU Time komanda komandinėje eilutėje
----------------------------------
=end

require "optparse"
require "json"

=begin
Used to select the next worker to which data is sent.
    actors: worker actor count
=end
class Selector
    def initialize(actors)
        @max_actors = actors
        @current = 0
    end
    
=begin
Returns the next number from range 0..actors-1, starting with 0. 
Resets when the current number reachers actors-1.
=end
    def get_next()
        if @current == @max_actors then
            @current = 0
        end 
        @current += 1
        return @current - 1
    end
end

=begin
Message object used as a payload when sending messages to/from distributor
    sender: sender actor identifier symbol
    payload: message data
=end
class Message
    attr_accessor :sender, :payload
    def initialize( sender, payload)
       
        @sender = sender
        @payload = payload
    end
end

=begin
Payload message object for communincation with workers
    id: worker id (index in worker array)
    data: actual payload from distributor
=end
class WorkerPayload
    attr_accessor :id, :data
    def initialize( id, data)
        
        @id = id
        @data = data
    end
end

=begin
Does some calculations and returns a value. Just wastes time.
=end
def CalculateValue( entry)
    number = entry.model.length + Integer(entry.year) + Integer(Float(entry.engineVolume))
    result = 1
    for i in 0..number
        for j in 0..number * 3
            result += i
        end
        result = i
    end
    return result
end

=begin
Option parsing for the program. 
Must pass 2 arguments:
    worker actor count (-c)
    JSON data file path (-f)
Arguments are saved in options struct, values can be accessed by:
options[:actors] - worker actor count
options[:file] - data file path
=end

options={}
OptionParser.new do |opts| 
    opts.banner = "Usage ruby egz_proj.rb [options]"
    opts.on("-a", "--actors COUNT", "Worker actor count"){ 
        |opt| options[:actors] = opt
    }

    opts.on("-f", "--file DATA_FILE", "Input data JSON file"){ 
        |opt| options[:file] = opt
    }

    opts.on("-h", "--help", "Show this message") {
        puts opts
        exit
    }
    options[:help] = opts.help
end.parse!

if options[:actors].nil? or options[:file].nil? then
    puts options[:help]
    exit 1
end


=begin
Open the data file and read it, validate actors argument.
Catch any error that may occur.
=end

begin
    file = File.open(options[:file])
    data =JSON.load(file)
rescue Errno::ENOENT
    puts "Error: data file not found, exitting..."
    exit 1
rescue JSON::ParserError
    puts "Error: data is not valid JSON, exitting..."
    exit 1
end

begin
    max_actors = Integer(options[:actors]) #total worker count as an integer
    if max_actors < 1 then
        raise ArgumentError
    end
rescue ArgumentError
    puts "Error: actor is not a number or illegal value, exitting..."
    exit 1
end

# workers = max_actors.times.map { 
#     Ractor.new{
  
#         distributor = Ractor.receive
        
#         msg = Message.new(:worker, nil)
        
#         while true do
#             entry = Ractor.receive
           
#             if entry.nil? then
#                 break
#             end

#             val = CalculateValue( entry)
#             next if val % 2 != 0 

#             entry.calculatedValue = val
#             msg.payload = entry
            
#             distributor.send(msg)
           
#         end
#         msg.payload = nil
#         distributor.send(msg)
#     }
# }


# selector = Selector.new(max_actors)
# distributor = Ractor.new(workers, selector, max_actors){ |workers, selector, max_actors|
#     count = 0
    
#     while count < max_actors do
#         # puts "count: #{count}"
#         msg = Ractor.receive
#         # puts msg.payload

#         case msg.sender
#         when :data
#             if msg.payload.nil?
#                 # puts "GOT NIL"
#                 workers.each{|worker| worker.send(nil)}
#             else
#                 # puts "GOT DATA FROM MAIN #{msg.payload["model"]}"
#                 current = selector.get_next()
#                 # puts "Send worker #{current}"
#                 workers[current].send(msg.payload)
#             end

#         when :worker
#             if msg.payload.nil?
#                 # puts "GOT NIL FROM WORKER, "
#                 count += 1
#             else
#                 # puts "GOT RESULT FROM WORKER"
#                 puts msg.payload
#             end
#         end 
#     end
# }

=begin
Data object, used in message payloads
=end
class Car
    attr_accessor :model, :year, :engineVolume, :calculatedValue
    def initialize(model, year, engineVolume)
        @model = model
        @year = year
        @engineVolume = engineVolume
        @calculatedValue = 0
    end

    def to_s()
        return "model: #{@model} year: #{year} engineVolume: #{engineVolume}"
    end
end

=begin
Logger actor, receives messages from Distributor and prints to a log file
Assumes received messages are strings
=end
class Logger
    def initialize(distributor)
        @distributor = distributor  
    end

    def run()
        Ractor.new(@distributor){ |dist|
        #clear old file
        File.delete("log.txt") if File.exist?("log.txt")
        log = File.open("log.txt", "a+")
            loop{
                recv = Ractor.receive
                break if recv.nil?
                #add timestamp to received message
                log.puts("#{Time.now.strftime("%Y-%m-%d %H:%M:%S.%L")}: #{recv}")
            }
        }
    end
end

=begin
Printer actor, receives an array of results from Distributor.
Prints data of every array element to a .txt file.
Assumes received data is an array of Car objects
=end
class Printer
    def initialize(distributor)
        @distributor = distributor  
    end
    def run()
        Ractor.new(@distributor){ |dist|
            file = File.open("results.txt", "w")
            file.puts("%-25s|%-10s|%-20s|%-20s|" % ["Modelis", "Metai", "Darbinis tūris", "Skaičiuota reikšmė"])
            file.puts("%s|%s|%s|%s|" % ["-"*25, "-"*10,"-"*20,"-"*20])
            
            recv = Ractor.receive
            recv.each{|car| 
                file.puts("%-25s|%10d|%20d|%20d|" % [car.model, car.year, car.engineVolume, car.calculatedValue])
            }
            msg = Message.new(:done, nil)
            dist.send(msg)
        }
    end
end

=begin
ResultAccumulator receives Car objects from Distributor, inserts in a sorted array.
Sorts by Car.year
=end
class ResultAccumulator
    def initialize(distributor)
        @distributor = distributor  
    end
   
    def run()
        Ractor.new(@distributor){ |dist|
            results = []
            loop{
                inserted = false
                recv = Ractor.receive
                break if recv.nil?

                for i in 0..results.length - 1
                    if results[i].year <= recv.year then
                        results.insert(i, recv)
                        inserted = true
                        break
                    end
                end
                next if inserted 
                
                #no place to insert, means the entry goes to the end of the array
                results.push(recv)
            }
            
            
            msg = Message.new(:results, results)
            dist.send(msg)
        }
    end
end

=begin
Worker receives messages from Distributor and does calculations.
Assumes the received payload is a Car object.
Wastes time calculating a value and assigns it to Car.calculatedValue and sends the object back to Distributor.
=end
class Worker
    def initialize(distributor)
        @distributor = distributor  
    end

    def run()
        Ractor.new(@distributor){ |dist|
            msg = Message.new(:worker, nil)

            loop{
                #receive Message with WorkerPayload as payload
                recv = Ractor.receive
                break if recv.nil?

                val = CalculateValue( recv.data)

                #entry filtering
                next if val % 2 != 0

                recv.data.calculatedValue = val

                msg.payload = recv
                dist.send(msg)
            }
            
            #worker finished
            msg.payload.data = nil
            dist.send(msg)
        }
    end

end

=begin
Distributor actor communicates with all other actors.
Uses Message objects as parameter to Ractor.send().
Receives messages from main thread, assumes Car object as payload.
    *Distrubutor sends nil to other actors as payload, which acts as an end condition for workers.
    *Worker actors use WorkerPayload as Message.payload (includes worker id)
        **Workers send nil as payload to Distributor to inform that they finished
=end
class Distributor
    def initialize(worker_count)
        @worker_count = worker_count
    end

    def run()
        Ractor.new(@worker_count){|worker_count|
            #create other actors, pass distributor as reference
            workers = []
                selector = Selector.new(worker_count)
                worker_count.times{
                
                worker = Worker.new(Ractor.current)
            
                workers.push(worker.run())
            }
            logger = Logger.new(Ractor.current).run()
            resultsAcc = ResultAccumulator.new(Ractor.current).run()
            printer = Printer.new(Ractor.current).run()

            #finished worker counter
            count = 0

            loop{
                recv = Ractor.receive

                #check Message sender symbol to determine the actor
                case recv.sender
                    
                #printer has finished writing to file, which means every actor is done
                when :done
                    break
                
                when :data
                    if recv.payload.nil?
                        logger.send("Visi duomenys nuskaityti, siunčiamos pabaigos žinutės visiems darbininkams")
                        workers.each{|worker| worker.send(nil)}
                    else
                        current = selector.get_next()
                        wp = WorkerPayload.new(current, recv.payload)
                        workers[current].send(wp)
                        logger.send("Gauta įrašas iš pagrindinio proceso (#{recv.payload.to_s}), siunčiamas darbininkui id: #{current}")
                    end
        
                when :worker
                    #WorkerPayload is assumed as the value of recv.payload
                    if recv.payload.data.nil?
                        logger.send("Darbininkas id: #{recv.payload.id} baigė darbą.")
                        count += 1
                    else
                        logger.send("Rezultatas iš darbininko id: #{recv.payload.id} (#{recv.payload.data.to_s}; #{recv.payload.data.calculatedValue}) siunčiamas kaupikliui")
                        resultsAcc.send(recv.payload.data)
                    end
                    
                    if count == worker_count then
                        logger.send("Visi darbininkai baigė darbą, siunčiamas užbaigimo signalas rezultatų kaupikliui.")
                        resultsAcc.send(nil)
                    end
                    
                when :results
                    logger.send("Gautas rezultatų sąrašas iš rezultatų kaupiklio ir persiųstas spausdintojui.")
                    printer.send(recv.payload)
                end 
              
            }

            logger.send("Skirstytuvas baigia darbą, siunčia pabaigos pranešimą žurnalo aktoriui.")
            logger.send(nil)
        }
        
    end
end



#run the distributor
distributor = Distributor.new(max_actors)
distributor = distributor.run()

#pass data to distributor
data.each { |entry|  
    car = Car.new(entry["model"], entry["year"], entry["enginevolume"])
    msg = Message.new(:data, car)
    distributor.send(msg)  
}

#all data send, queue end condition message
distributor.send(Message.new(:data, nil))
distributor.take()


