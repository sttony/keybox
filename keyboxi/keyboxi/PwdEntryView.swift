

import SwiftUI

struct PwdEntryView: View {
    var body: some View {
        VStack {
            HStack{
                Text("Password")
                SecureField("", text: .constant(""))
            }
            
        }
        .padding()
        .onAppear(){
        }
    }
}

#Preview {
    PwdEntryView()
}
