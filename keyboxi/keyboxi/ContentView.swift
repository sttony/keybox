

import SwiftUI

struct ContentView: View {
    var m_rg = ORandomGenerator(1)
    @State var te = ""
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundStyle(.tint)
            
            TextField("test", text:$te)
            Button(action: {
                te = "hello \(m_rg!.getNextInt())"
            }){
                Text("Generate")
            }
            
        }
        .padding()
        .onAppear(){
            let keys: [UInt8] = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                                0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38]
           
            let key = Data(bytes:keys)
            let ivs: [UInt8] = [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08]
            let iv = Data(bytes: ivs)
            m_rg?.initWithKey(key, iv: iv)
        }
    }
}

#Preview {
    ContentView()
}
